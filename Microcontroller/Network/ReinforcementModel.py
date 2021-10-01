import math
import random
from torchsummary import summary
import numpy as np
import torch
import torch.optim as optim
import torch.nn.functional as F

from Microcontroller.Network.ReinforcementlearningElements import RewardFunctions
from Microcontroller.Network.ReinforcementlearningElements.ReplayMemory import ReplayMemory
from Simulation.Main.support.logger import logger


def transform_state(state):
    image, array, _ = state.get_formatted()
    image = np.expand_dims(image, axis=0)
    array = [0 if _ is None else _ for _ in array]
    return torch.from_numpy(image).float(), torch.tensor([[array]]).float()


class ReinforcementModel:
    # This is a wrapper for reinforcement learning
    # The class loads a model and prepares it for training.
    # https://pytorch.org/tutorials/intermediate/reinforcement_q_learning.html
    # It uses a target network and a policy network. The policy network is optimized
    # and then the target network is replaced with the policy after a few time steps,
    def __init__(self, dim_features, height, width, n_actions, **kwargs):
        # The parameters are the following
        #   dim_features    - the dimension of the feature vector.
        #   height, width   - the parameters of the input picture.
        #   n_actions       - number of actions on the output of the network
        #   **kwargs        - This will later contain additional parameters, such as the model.
        #
        # Other parameters:
        #   Gamma and eps are parameters fro the reinforcement learning
        #   Eps is there to determine the exploration parameter fo the agent.
        #   TARGET_UPDATE defines when to update the target network with the policy network
        #   A reward function is loaded into the reward variable
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        logger.info(f'Device is {self.device}')
        self.BATCH_SIZE = 256
        self.GAMMA = 0.999
        self.EPS_START = 0.9
        self.EPS_END = 0.05  # with 2 choices this means 0.4/2 -> 20% are wrong random choices (should be tolerable)
        self.EPS_DECAY = 200  # This should equal a couple of short runs
        self.TARGET_UPDATE = 75
        self.steps_done = 0
        self.time_step = 0
        self.n_training = 0
        self.rewards = []
        self.rewards.append([])
        self.prev_state = None
        self.action = -1
        self.n_actions = n_actions
        if kwargs.get('model') is None:
            kwargs['model'] = CNNwRNN

        self.policy_net = kwargs['model'](dim_features, height, width, self.n_actions).to(self.device)
        self.target_net = kwargs['model'](dim_features, height, width, self.n_actions).to(self.device)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        self.target_net.eval()
        self.optimizer = optim.RMSprop(self.policy_net.parameters(), lr=0.001)
        self.memory = ReplayMemory(10000)
        self.reward = RewardFunctions.inline_reward
        summary(self.target_net, [(1, height, width), (1, 1, dim_features)])

    def predict(self, state):
        # Select an action
        self.prev_state = state
        image, features = transform_state(state)
        self.action = self.select_action(image.to(self.device), features.to(self.device))
        # Returning an integer instead of the tensor containing that integer
        logger.debug(self.action.item())
        return self.action.item()

    def select_action(self, image, features):
        sample = random.random()
        eps_threshold = self.EPS_END + (self.EPS_START - self.EPS_END) * \
                        math.exp(-1. * self.steps_done / self.EPS_DECAY)
        self.steps_done += 1
        logger.debug(f'Sample: {str(sample)}, Threshold: {str(eps_threshold)}')
        if self.steps_done % 10 == 0:
            logger.debug(f'Epoch: {self.steps_done}')
        if sample > eps_threshold:
            with torch.no_grad():
                # t.max(1) will return largest column value of each row.
                # second column on max result is index of where max element was
                # found, so we pick action with the larger expected reward.
                return self.policy_net(image, features).max(1)[1].view(1, 1)
        else:
            return torch.tensor([[random.randrange(self.n_actions)]], device=self.device, dtype=torch.long)

    def optimize(self, new_state, prev_state=None, action=None, reward=None):
        if prev_state is not None and action is not None:
            self.prev_state = prev_state
            self.action = torch.tensor([[action]], dtype=torch.int64)
        # Calculates the rewards, saves the state and the transition.
        # After TARGET_UPDATE steps, replaces the target network's weights with the policy network's
        if reward is None:
            reward = self.reward(prev_state=self.prev_state, new_state=new_state)
        # if random.random() > 0.5:
        #     print(action, reward)
        self.rewards[self.n_training].append(reward)
        reward = torch.tensor([reward], device=self.device)
        # Store the transition in memory
        prev_image, prev_features = transform_state(self.prev_state)
        new_image, new_features = transform_state(new_state)
        self.memory.push(prev_image.to(self.device), prev_features.to(self.device), self.action.to(self.device),
                         new_image.to(self.device), new_features.to(self.device), reward.to(self.device))

        # Perform one step of the optimization (on the target network)
        self.optimize_model()
        self.time_step += 1
        # Update the target network, copying all weights and biases in DQN
        if self.time_step % self.TARGET_UPDATE == 0:
            self.target_net.load_state_dict(self.policy_net.state_dict())
        return reward

    def optimize_ESP(self):
        self.optimize_model()
        self.time_step += 1
        # Update the target network, copying all weights and biases in DQN
        if self.time_step % self.TARGET_UPDATE == 0:
            self.target_net.load_state_dict(self.policy_net.state_dict())

    def optimize_model(self):
        if len(self.memory) < self.BATCH_SIZE:
            return

        logger.debug('Batch optimization started')  # This can be slow
        transitions = self.memory.sample(self.BATCH_SIZE)
        # Transpose the batch (see https://stackoverflow.com/a/19343/3343043 for
        # detailed explanation). This converts batch-array of Transitions
        # to Transition of batch-arrays.
        batch = Transition(*zip(*transitions))

        # Compute a mask of non-final states and concatenate the batch elements
        # (a final state would've been the one after which simulation ended)
        non_final_mask_image = torch.tensor(tuple(map(lambda s: s is not None,
                                                      batch.next_image)), device=self.device, dtype=torch.bool)
        non_final_next_states_image = torch.cat([s for s in batch.next_image
                                                 if s is not None])
        non_final_mask_features = torch.tensor(tuple(map(lambda s: s is not None,
                                                         batch.next_features)), device=self.device, dtype=torch.bool)
        non_final_next_state_features = torch.cat([s for s in batch.next_features
                                                   if s is not None])
        image_batch = torch.cat(batch.state_image)
        features_batch = torch.cat(batch.state_features)
        action_batch = torch.cat(batch.action)
        reward_batch = torch.cat(batch.reward)

        # Compute Q(s_t, a) - the model computes Q(s_t), then we select the
        # columns of actions taken. These are the actions which would've been taken
        # for each batch state according to policy_net
        state_action_values = self.policy_net(image_batch, features_batch).gather(1, action_batch)

        # Compute V(s_{t+1}) for all next states.
        # Expected values of actions for non_final_next_states are computed based
        # on the "older" target_net; selecting their best reward with max(1)[0].
        # This is merged based on the mask, such that we'll have either the expected
        # state value or 0 in case the state was final.
        next_state_values = torch.zeros(self.BATCH_SIZE, device=self.device)
        next_state_values[non_final_mask_image] = \
            self.target_net(non_final_next_states_image, non_final_next_state_features).max(1)[
                0].detach()
        # Compute the expected Q values
        expected_state_action_values = (next_state_values * self.GAMMA) + reward_batch

        # Compute Huber loss
        loss = F.smooth_l1_loss(state_action_values.float(), expected_state_action_values.unsqueeze(1).float())

        # Optimize the model
        self.optimizer.zero_grad()
        loss.backward()
        for param in self.policy_net.parameters():
            param.grad.data.clamp_(-1, 1)
        self.optimizer.step()
        logger.debug('Batch optimization finished')

    def reset(self):
        logger.warning('Not plotting')
        # self.plot_rewards()
        self.n_training += 1
        self.rewards.append([])

    def plot_rewards(self):
        plt.figure(1)
        plt.clf()
        plt.title('Training...')
        plt.xlabel('Time')
        plt.ylabel('Reward')
        cumulative = []
        for i in range(len(self.rewards[self.n_training])):
            if i == 0:
                cumulative.append(self.rewards[self.n_training][i])
            else:
                cumulative.append(self.rewards[self.n_training][i] + cumulative[-1])
        x = np.linspace(2.0, len(self.rewards[self.n_training]), num=len(self.rewards[self.n_training]))
        plt.plot(x, cumulative)
        plt.show()
        plt.pause(0.001)  # pause a bit so that plots are updated

    def save_model(self, path):
        model = self.target_net.state_dict()
        torch.save(model, path)

    def load_model(self, path, dim_features, image_height, image_width, n_actions, **kwargs):
        if kwargs.get('model') is None:
            kwargs['model'] = CNNwRNN
        model = kwargs['model'](dim_features, image_height, image_width, n_actions)
        model.load_state_dict(torch.load(path))
        model.eval()
        self.target_net.load_state_dict(model.state_dict())
        self.policy_net.load_state_dict(model.state_dict())
