import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import plotly.graph_objects as go
from plotly.offline import iplot
from wordcloud import WordCloud, STOPWORDS
from textblob import TextBlob
from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer

phi = (1 + 5**0.5) / 2 # == 1.6180...

eps = np.finfo(float).eps

vader = SentimentIntensityAnalyzer()

mpl.rcParams['figure.dpi'] = 256

market_breaks = [
  dict(bounds=['sat', 'mon']),
  dict(pattern='hour', bounds=[16, 9.5]),
]

def split(str, sep=','):
  return str.split(sep)

def join(str, sep=','):
  return sep.join(str)

def get_sentiment(text):
  b = TextBlob(text)
  v = vader.polarity_scores(text)
  return dict(
    bpol=b.polarity, bsub=b.subjectivity,
    vpol=v['compound'], vpos=v['pos'], vneu=v['neu'], vneg=v['neg'],
  )

def mkmargin(size=15):
  m = size
  return go.layout.Margin(l=m, t=m*phi, r=m*phi, b=m)

def mklayout(margin=None, **kwargs):
  if margin is None: margin = mkmargin(15)
  return go.Layout(margin=margin, **kwargs)

def plot_wordcloud(text, width=800, height=800, **kwargs):
  if not isinstance(text, str):
    text = ' '.join(text)
  text = text.lower()
  wordcloud = WordCloud(width=width, height=height, background_color='white',
                        stopwords=STOPWORDS, min_font_size=14, **kwargs)
  wordcloud = wordcloud.generate(text)
  plt.imshow(wordcloud)
  plt.axis('off')
  plt.show()

def plot_multi_series(*series, labels=None, ylim=None):
  if labels is None: labels = [f'series {i}' for i in range(len(series))]
  axs = [go.Scatter(x=xs.index, y=xs, name=name) for xs, name in zip(series, labels)]
  fig = go.Figure(data=axs, layout=mklayout(
    xaxis=dict(rangeslider_visible=True),
    yaxis=dict(fixedrange=False, range=ylim),
  ))
  fig.show()

def plot_ohlc(ohlc):
  ax1 = go.Candlestick(x=ohlc.index, open=ohlc.Open, high=ohlc.High, low=ohlc.Low, close=ohlc.Close)
  fig = go.Figure(data=[ax1], layout=mklayout(
    xaxis=dict(rangeslider_visible=True, rangebreaks=market_breaks),
    yaxis=dict(fixedrange=False),
  ))
  fig.show()
