from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer

import pandas as pd
import ai.datasets.loader as loader

sid_obj = SentimentIntensityAnalyzer()

def sentiment_vader(sentence):
    sentiment_dict = sid_obj.polarity_scores(sentence)
    negative = sentiment_dict['neg']
    neutral = sentiment_dict['neu']
    positive = sentiment_dict['pos']
    compound = sentiment_dict['compound']

    if compound >= 0.05:
        overall_sentiment = "Positive"
    elif compound <= - 0.05:
        overall_sentiment = "Negative"
    else:
        overall_sentiment = "Neutral"

    return negative, neutral, positive, compound, overall_sentiment

list_of_words = loader.get_memes_dataframe()
valid_sentiment = []
for idx, text in enumerate(list_of_words["CaptionText"]):
    print(idx)
    sentiment = sentiment_vader(' '.join(text))
    print(sentiment)
    valid_sentiment.append(sentiment)

sentiment = pd.DataFrame(valid_sentiment)
sentiment.to_csv('sentiment.csv', index_label='id')
