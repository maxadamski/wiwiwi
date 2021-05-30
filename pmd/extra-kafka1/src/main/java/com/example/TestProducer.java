package com.example;

import com.opencsv.CSVReader;
import com.opencsv.exceptions.CsvException;
import com.opencsv.exceptions.CsvValidationException;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Properties;
import java.util.concurrent.TimeUnit;

public class TestProducer {
    enum Mode {
        NONE, ANALYZE, GENERATE
    }

    static String input;
    static String topic = "kafka-to-ss";
    static String bootstrap = "0.0.0.0:6667";
    static String ts_format = "yyyy-MM-dd'T'HH:mm:ss";
    static int ts_col = 0;
    static float speedup = 1;
    static int skip = 1;
    static Mode mode = Mode.NONE;

    static void printUsage(int exit_code) {
        System.out.println("usage: java -jar KafkaProducer.jar\n"
                + "  --input INPUT\n"
                + "  --topic TOPIC\n"
                + "  --mode (generate | analyze)\n"
                + "  --bootstrap ADDRESS\n"
                + "      default: 0.0.0.0:6667\n"
                + "  --skip N_LINES (int)\n"
                + "      default: 1\n"
                + "      skips first N lines in the input .csv files\n"
                + "  --speedup N_TIMES_FASTER (float)\n"
                + "      default: 1\n"
                + "      produces messages N_TIMES_FASTER than the specified date\n"
                + "      send date is calculated as follows:\n"
                + "         first message ts + (next message ts - first message ts) / speedup\n"
                + "  --ts_format FORMAT\n"
                + "      default: \"yyyy-MM-dd'T'HH:mm:ss\"\n"
                + "      SimpleDateFormat-compatible timestamp format\n"
                + "  --ts_col INDEX (int)\n"
                + "      default: 0 (columns are zero-indexed)\n"
        );
        System.exit(exit_code);
    }

    static void parseArgs(String[] args) {
        int i = 0;
        while (i < args.length - 1) {
            switch (args[i]) {
                case "--input": input = args[++i]; break;
                case "--bootstrap": bootstrap = args[++i]; break;
                case "--topic": topic = args[++i]; break;
                case "--skip": skip = Integer.parseInt(args[++i]); break;
                case "--speedup": speedup = Float.parseFloat(args[++i]); break;
                case "--ts_col": ts_col = Integer.parseInt(args[++i]); break;
                case "--ts_format": ts_format = args[++i]; break;
                case "--mode":
                    switch (args[++i]) {
                        case "analyze": mode = Mode.ANALYZE; break;
                        case "generate": mode = Mode.GENERATE; break;
                        default: printUsage(1);
                    }
                    break;
                case "--help": printUsage(0);
                default:
                    System.err.println("Unknown option " + args[i]);
                    printUsage(1);
            }
            i++;
        }

        if (mode == Mode.NONE) {
            System.err.println("Please provide --mode");
            printUsage(1);
        }
    }

    static Date maybeParseDate(String string, SimpleDateFormat format) {
        try {
            return format.parse(string);
        } catch (ParseException e) {
            return null;
        }
    }

    public static void main(String[] args) {
        parseArgs(args);

        var folder = new File(input);
        var paths = Arrays.stream(folder.listFiles()).map(File::getAbsolutePath).toArray(String[]::new);
        Arrays.sort(paths);

        var dateFormat = new SimpleDateFormat(ts_format);
        Date firstDate, lastDate;
        try (var firstReader = new CSVReader(new FileReader(paths[0]));
             var lastReader = new CSVReader(new FileReader(paths[paths.length - 1]))) {
            firstReader.skip(skip);
            var firstLine = firstReader.readNext();
            firstDate = dateFormat.parse(firstLine[ts_col]);

            String[] lastLine = null;
            while (true) {
                var line = lastReader.readNext();
                if (line == null) break;
                lastLine = line;
            }
            if (lastLine == null) {
                System.err.println("File " + paths[paths.length - 1] + "had 0 rows!");
                return;
            }
            lastDate = dateFormat.parse(lastLine[ts_col]);
        } catch (IOException | ParseException | CsvException e) {
            e.printStackTrace();
            return;
        }

        var dateDiff = (lastDate.getTime() - firstDate.getTime());
        if (mode == Mode.ANALYZE) {
            System.out.println("first timestamp: " + firstDate);
            System.out.println("last  timestamp: " + lastDate);
            System.out.println("difference in milliseconds: " + dateDiff);
            return;
        }

        var SERIALIZER = "org.apache.kafka.common.serialization.StringSerializer";
        var props = new Properties();
        props.put("bootstrap.servers", bootstrap);
        props.put("acks", "all");
        props.put("retries", 0);
        props.put("batch.size", 16384);
        props.put("linger.ms", 1);
        props.put("buffer.memory", 33554432);
        props.put("key.serializer", SERIALIZER);
        props.put("value.serializer", SERIALIZER);

        try (var producer = new KafkaProducer<>(props)) {
            var firstMessage = true;
            long firstSend = System.currentTimeMillis();
            for (var path : paths) {
                try (var reader = new CSVReader(new FileReader(path))) {
                    reader.skip(skip);
                    while (true) {
                        var row = reader.readNext();
                        if (row == null) break;
                        Date sendDate = maybeParseDate(row[ts_col], dateFormat);
                        if (sendDate == null) continue;
                        var text = String.join(",", row);
                        var timeUntil = firstSend + (long) ((sendDate.getTime() - firstSend) / speedup) - System.currentTimeMillis();
                        if (timeUntil > 0 && !firstMessage) {
                            TimeUnit.MILLISECONDS.sleep(timeUntil);
                        }
                        producer.send(new ProducerRecord<>(topic, String.valueOf(text.hashCode()), text));
                        if (firstMessage) {
                            firstSend = System.currentTimeMillis();
                            firstMessage = false;
                        }
                    }
                } catch (IOException | InterruptedException | CsvValidationException e) {
                    e.printStackTrace();
                    return;
                }
            }
        }
    }
}

