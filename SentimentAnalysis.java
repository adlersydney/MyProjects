import java.io.BufferedReader;
import java.io.FileReader;
import java.util.Scanner;

/**
 * @author sydneyadler
 * @version 2017.11.30
 */
public class SentimentAnalysis {
    
    private BinarySearchTree<WordAndScore> tree;
    
    /**
     * Construct sentimentAnalysis and initialize the tree
     */
    public SentimentAnalysis() {
        tree = new BinarySearchTree<WordAndScore>();
    }
    
    public SentimentAnalysis(String review) {
        // TODO Auto-generated constructor stub
    }

    /**
     * Create a scanner of the file name get each line of the file and store each
     * word as a WordAndScore and insert into the tree. O(n^2 log n) reading in the
     * file, n words, and inserting into the tree
     * 
     * @param file
     *            movieReview file
     * 
     *            does not need a test
     */
    public void index(String file) {
        try (BufferedReader br = new BufferedReader(
            new FileReader(file))) {
            String line;
            line = br.readLine();
            while ((line = br.readLine()) != null) {
                String[] parts = line.replaceAll("\\p{Punct}", "")
                    .toLowerCase().split("\\s+");
                // remove punctuation, change to lowercase and put all words in parts
                addTree(parts);
            }
        }
        catch (Exception e) {
            System.out.println("Error" + e);
        }
        
    }
    
    /**
     * Create a scanner for the user to input review/word. Match up the words in the
     * existing nodes with the average scores, re-average all of the scores in the
     * string and return the average score of the entire string.
     * 
     * @return score
     * 
     *         O(n log n) get user input and find the word
     */
    
    public double score() {
        Scanner input = new Scanner(System.in);
        System.out.println("Enter a review: ");
        String userInput = input.nextLine();
        return wordScore(userInput);
    }
    
    /**
     * @return the tree
     */
    public BinarySearchTree<WordAndScore> getTree() {
        return tree;
    }
    
    /**
     * method that is called by the index method and takes array of Strings and
     * inserts into the tree. O(n log n)
     * 
     * @param words
     *            - array of Strings
     * 
     */
    public void addTree(String[] words) {
        int score = Integer.parseInt(words[0]);
        for (int i = 1; i < words.length; i++) {
            insertTree(words[i], score);
        }
    }
    
    /**
     * takes a String and a score and inserts into the tree. O(log n)
     * 
     * @param word
     *            - to be rated
     * @param score
     *            - of the String
     */
    public void insertTree(String word, int score) {
        WordAndScore ws = new WordAndScore(word);
        WordAndScore findWS = tree.find(ws);
        if (findWS == null) {
            tree.insert(ws);
            ws.updateScore(score);
        }
        else {
            findWS.updateScore(score);
        }
        
    }
    
    /**
     * takes the review and scores it
     * 
     * @param w
     *            word String
     * @return returns the score of the String
     */
    public double wordScore(String w) {
        String[] parts = w.replaceAll("\\p{Punct}", "").toLowerCase()
            .split("\\s+");
        double total = 0;
        int numWords = 0;
        for (int i = 0; i < parts.length; i++) {
            WordAndScore ws = tree.find(new WordAndScore(parts[i]));
            if (ws != null) {
                total = total + ws.getAverageScore();
                numWords++;
            }
        }
        
        return (total / numWords);
    }
    
}
