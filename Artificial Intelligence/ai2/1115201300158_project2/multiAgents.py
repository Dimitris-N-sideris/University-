# multiAgents.py
# --------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent


def manhattanChanged(position, goal):
    "The Manhattan distance heuristic for a PositionSearchProblem"
    xy1 = position
    xy2 = goal
    return abs(position[0] - goal[0]) + abs(position[1] - goal[1])



class ReflexAgent(Agent):
    """
      A reflex agent chooses an action at each choice point by examining
      its alternatives via a state evaluation function.

      The code below is provided as a guide.  You are welcome to change
      it in any way you see fit, so long as you don't touch our method
      headers.
    """



    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {North, South, West, East, Stop}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

       # "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action)
        newPos = successorGameState.getPacmanPosition()
        newFood = successorGameState.getFood()
        newGhostStates = successorGameState.getGhostStates()

        FoodWeight = 15
        GhostWeight = 5
        ScaredWeight = 5
        StopPenalty = 3000
        leftoverFood = newFood.asList()
        # the distance from pacman to each ghost
        ghostDistances = [manhattanChanged(newPos, ghost.getPosition()) for ghost in newGhostStates]
        foodDistances = [manhattanChanged(newPos, food) for food in leftoverFood]
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

        score = successorGameState.getScore()
        if successorGameState.isWin():
            return score + 500

        closestGhost = min(ghostDistances)
        if closestGhost == ghostDistances[0]:
            pos = 0
        else:
            pos = 1
        if closestGhost < 2:
            if newScaredTimes[pos] > closestGhost + 1:
                score += ScaredWeight/(closestGhost + 1) * GhostWeight
            else:
                score -= GhostWeight/(closestGhost + 1) * GhostWeight

        closestFood = min(foodDistances)

        if action == Directions.STOP:
            score -= StopPenalty
        score += FoodWeight/closestFood

        return score


def scoreEvaluationFunction(currentGameState):
    """
      This default evaluation function just returns the score of the state.
      The score is the same one displayed in the Pacman GUI.

      This evaluation function is meant for use with adversarial search agents
      (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
      This class provides some common elements to all of your
      multi-agent searchers.  Any methods defined here will be available
      to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

      You *do not* need to make any changes here, but you can if you want to
      add functionality to all your adversarial search agents.  Please do not
      remove anything, however.

      Note: this is an abstract class: one that should not be instantiated.  It's
      only partially specified, and designed to be extended.  Agent (game.py)
      is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
      Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
          Returns the minimax action from the current gameState using self.depth
          and self.evaluationFunction.

          Here are some method calls that might be useful when implementing minimax.

          gameState.getLegalActions(agentIndex):
            Returns a list of legal actions for an agent
            agentIndex=0 means Pacman, ghosts are >= 1

          gameState.generateSuccessor(agentIndex, action):
            Returns the successor game state after an agent takes an action

          gameState.getNumAgents():
            Returns the total number of agents in the game
        """
        agent_number = gameState.getNumAgents()
        max_limit = 100000000
        min_limit = -100000000
        def max_value(state, depth):

            pacmanMoves = state.getLegalActions(0)
            if not pacmanMoves or depth > self.depth:
                return self.evaluationFunction(state)
            max_cost = min_limit
            action = pacmanMoves[0]
            for move in pacmanMoves:
                stateAfterMove = state.generateSuccessor(0, move)
                cost = min_value(stateAfterMove, depth, 1)
                if cost > max_cost:
                    action = move
                    max_cost = cost

            if depth == 1:
                return action
            else:
                return max_cost

        def min_value(state, depth, agent_id):

            ghostMoves = state.getLegalActions(agent_id)
            if not ghostMoves:
                return self.evaluationFunction(state)

            min_cost = max_limit
            for move in ghostMoves:
                stateAfterGhostMove = state.generateSuccessor(agent_id, move)
                if agent_id == agent_number - 1:
                    cost = max_value(stateAfterGhostMove, depth + 1)
                else:
                    cost = min_value(stateAfterGhostMove, depth, agent_id + 1)

                if min_cost > cost:
                    min_cost = cost

            return min_cost

        return max_value(gameState, 1)

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
      Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
          Returns the minimax action using self.depth and self.evaluationFunction
        """
        agent_number = gameState.getNumAgents()
        max_limit = 100000000
        min_limit = -100000000

        def max_value(state, depth, max_limiter, min_limiter):

            pacmanMoves = state.getLegalActions(0)
            if  not pacmanMoves or depth > self.depth:
                return self.evaluationFunction(state)
            max_cost = min_limit
            action = pacmanMoves[0]
            for move in pacmanMoves:
                stateAfterMove = state.generateSuccessor(0, move)
                cost = min_value(stateAfterMove, depth, 1, max_limiter, min_limiter)
                if cost > max_cost:
                    action = move
                    max_cost = cost
                if max_limiter < max_cost:
                    max_limiter = max_cost
                if max_cost > min_limiter:
                    return max_cost

            if depth == 1:
                return action
            else:
                return max_cost

        def min_value(state, depth, agent_id,max_limiter, min_limiter):

            ghostMoves = state.getLegalActions(agent_id)
            if  not ghostMoves:
                return self.evaluationFunction(state)

            min_cost = max_limit
            for move in ghostMoves:
                stateAfterGhostMove = state.generateSuccessor(agent_id, move)
                if agent_id == agent_number - 1:
                    cost = max_value(stateAfterGhostMove, depth + 1, max_limiter, min_limiter)
                else:
                    cost = min_value(stateAfterGhostMove, depth, agent_id + 1, max_limiter, min_limiter)

                if min_cost > cost:
                    min_cost = cost

                if min_cost < min_limiter:
                    min_limiter = min_cost

                if max_limiter > min_cost:
                    return min_cost

            return min_cost

        return max_value(gameState, 1, min_limit, max_limit)

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
          Returns the expectimax action using self.depth and self.evaluationFunction

          All ghosts should be modeled as choosing uniformly at random from their
          legal moves.
        """
        agent_number = gameState.getNumAgents()
        max_limit = 100000000
        min_limit = -100000000

        def max_value(state, depth):

            pacmanMoves = state.getLegalActions(0)
            if not pacmanMoves or depth > self.depth:
                return self.evaluationFunction(state)
            max_cost = min_limit
            action = pacmanMoves[0]
            for move in pacmanMoves:
                stateAfterMove = state.generateSuccessor(0, move)
                cost = min_value(stateAfterMove, depth, 1)
                if cost > max_cost:
                    action = move
                    max_cost = cost

            if depth == 1:
                return action
            else:
                return max_cost

        def min_value(state, depth, agent_id):

            ghostMoves = state.getLegalActions(agent_id)
            if not ghostMoves:
                return self.evaluationFunction(state)

            randomFactor = 1.0 / len(ghostMoves)
            sum = 0
            for move in ghostMoves:

                stateAfterGhostMove = state.generateSuccessor(agent_id, move)
                if agent_id == agent_number - 1:
                    cost = max_value(stateAfterGhostMove, depth + 1)
                else:
                    cost = min_value(stateAfterGhostMove, depth, agent_id + 1)

                sum += randomFactor*cost

            return sum

        return max_value(gameState, 1)

def betterEvaluationFunction(currentGameState):
    """
      Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
      evaluation function (question 5).

      DESCRIPTION: <write something here so we know what you did>
    """

    newPos = currentGameState.getPacmanPosition()
    newFood = currentGameState.getFood()
    newGhostStates = currentGameState.getGhostStates()

    FoodWeight = 15
    GhostWeight = 5
    ScaredWeight = 5
    leftoverFood = newFood.asList()
    # the distance from pacman to each ghost
    ghostDistances = [manhattanChanged(newPos, ghost.getPosition()) for ghost in newGhostStates]
    foodDistances = [manhattanChanged(newPos, food) for food in leftoverFood]
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]

    score = currentGameState.getScore()
    if currentGameState.isWin():
        return score + 500

    closestGhost = min(ghostDistances)
    if closestGhost == ghostDistances[0]:
        pos = 0
    else:
        pos = 1
    if closestGhost < 2:
        if newScaredTimes[pos] > closestGhost + 1:
            score += ScaredWeight / (closestGhost + 1) * GhostWeight
        else:
            score -= GhostWeight / (closestGhost + 1) * GhostWeight

    closestFood = min(foodDistances)

    score += FoodWeight / closestFood

    return score

# Abbreviation
better = betterEvaluationFunction

