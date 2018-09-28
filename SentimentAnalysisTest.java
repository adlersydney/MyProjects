import student.TestCase;

/**
 * @author sydneyadler
 * @version 2017.11.30
 */
public class SentimentAnalysisTest extends TestCase {
    private SentimentAnalysis analyze;
    double                    s;
    
    public void setUp() {
        analyze = new SentimentAnalysis();
    }
    
    /**
     * tests Index
     */
    public void testIndex() {
        analyze.index("movieReviewTests.txt");
        BinarySearchTree<WordAndScore> tree = analyze.getTree();
        
        WordAndScore w1 = tree.find(new WordAndScore("boring"));
        assertEquals(w1.getAverageScore(), 2, 0.1);
        WordAndScore w2 = tree.find(new WordAndScore("quiet"));
        assertEquals(w2.getAverageScore(), 4, 0.1);
        WordAndScore w3 = tree.find(new WordAndScore("funny"));
        assertEquals(w3.getAverageScore(), 4, 0.1);
        
    }
    
    /**
     * tests Score
     */
    public void testScore() {
        analyze.index(
            "//Users//sydneyadler//Desktop//LEHIGH//Sophomore Year (2017-2018)//Fall 2017//CSE17//SentimentAnalysis//movieReviewTests.txt");
        s = analyze.wordScore("Quiet, boring story");
        
        // type in "Quiet, boring story"
        assertEquals(s, 3, 0.01);
        s = analyze.wordScore("Funny and cool");
        // type in "Funny and cool"
        assertEquals(s, 3.833, 0.01);
    }
    
    /**
     * tests addTree method
     */
    public void testAddTree() {
        String[] arr = new String[4];
        arr[0] = "1";
        arr[1] = "Boring";
        arr[2] = "stupid";
        arr[3] = "movie";
        
        analyze.addTree(arr);
        BinarySearchTree<WordAndScore> tree = analyze.getTree();
        WordAndScore word1 = tree.find(new WordAndScore("Boring"));
        WordAndScore word2 = tree.find(new WordAndScore("stupid"));
        WordAndScore word3 = tree.find(new WordAndScore("movie"));
        
        assertEquals(word1.getAverageScore(), 1, .01);
        assertEquals(word2.getAverageScore(), 1, .01);
        assertEquals(word3.getAverageScore(), 1, .01);
    }
    
    /**
     * tests insertTree method
     */
    public void testInsertTree() {
        analyze.insertTree("Funny", 3);
        analyze.insertTree("Funny", 4);
        
        assertEquals(analyze.getTree().find(new WordAndScore("Funny"))
            .getAverageScore(), 3.5, .01);
        analyze.insertTree("Funny", 2);
        assertEquals(analyze.getTree().find(new WordAndScore("Funny"))
            .getAverageScore(), 3, .01);
    }
    
}
