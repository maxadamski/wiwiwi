package put.ai.games.ourplayer;

import java.util.*;

import put.ai.games.game.Board;
import put.ai.games.game.Move;
import put.ai.games.game.Player;
import put.ai.games.game.Player.Color;


public class OurPlayer extends Player {

    class MyMove {
        Move move;
        Double value;

        MyMove(Move move, Double value) {
            this.move = move;
            this.value = value;
        }
    }

    static Random random = new Random(0xCAFFE);
    int boardSize = 6;
    long startTime = 0;
    long timeLimit = 10*1000;

    boolean timeout() {
        return System.currentTimeMillis() - startTime >= timeLimit;
    }

    MyMove randomMove(Board board, Color color) {
        List<Move> moves = board.getMovesFor(color);
        Move move = moves.get(random.nextInt(moves.size()));
        return new MyMove(move, score(board, color));
    }

    double score(Board board, Color color) {
        if (board.getWinner(color) == color) return Double.MAX_VALUE;
        int[] rows = new int[boardSize];
        int[] cols = new int[boardSize];
        int[] opprows = new int[boardSize];
        int[] oppcols = new int[boardSize];
        for (int i = 0; i < boardSize; i++) {
            for (int j = 0; j < boardSize; j++) {
                if (board.getState(i, j) == color) {
                    rows[i] += 1;
                    cols[i] += 1;
                } else if (board.getState(i, j) != Color.EMPTY) {
                    opprows[i] += 1;
                    oppcols[j] += 1;
                }
            }
        }

        double res = 0.0;
        for (int i = 0; i < boardSize; i++) {
            res += Math.pow(2, Math.min(rows[i], 5)) - 1;
            res += Math.pow(2, Math.min(cols[i], 5)) - 1;
            res -= Math.pow(2, opprows[i]) - 1;
            res -= Math.pow(2, oppcols[i]) - 1;
        }

        return res;
    }

    MyMove negamax(Board board, Color color, int depth, double alpha, double beta) {
        Color nextColor = color == Color.PLAYER1 ? Color.PLAYER2 : Color.PLAYER1;
        List<Move> nextMoves = board.getMovesFor(color);
        Collections.shuffle(nextMoves);
        if (depth == 0 || nextMoves.isEmpty() || timeout())
            return new MyMove(null, score(board, color));

        MyMove best = null;
        for (Move move : nextMoves) {
            board.doMove(move);
            MyMove next = negamax(board, nextColor, depth - 1, -beta, -alpha);
            board.undoMove(move);
            if (next.value == null) continue;
            next.value *= -1.0;
            next.move = move;
            if (best == null || next.value > best.value) best = next;
            alpha = Math.max(best.value, alpha);
            if (best.value >= beta) return best;
        }

        return best;
    }

    MyMove ids(Board board, Color color, int depthLimit) {
        MyMove best = randomMove(board, color);
        for (int depth = 1; depth <= depthLimit && !timeout(); depth++) {
            MyMove move = negamax(board, color, depth, Double.MIN_VALUE, Double.MAX_VALUE);
            if (move.value > best.value) best = move;
        }

        System.out.println("("+best.value+")\t"+best.move);
        return best;
    }

    @Override
    public Move nextMove(Board board) {
        // give some extra time for returning the move
        boardSize = board.getSize();
        timeLimit = getTime() - 500;
        startTime = System.currentTimeMillis();
        MyMove move = ids(board, getColor(), 20);
        board.doMove(move.move);
        return move.move;
    }

    @Override
    public String getName() {
        return "Monte 123456";
    }

    public static void main(String[] args) {
    }

}

