from flask import Flask, session
from flask import request
from flask import jsonify
import openai
import numpy as np
import json
import sys

if len(sys.argv) < 2:
    print("Api key missing")
    exit();

api_key = sys.argv[1]

app = Flask(__name__)
sentiments_list = {'angry': 0, 'annoyed': 0, 'frustrated': 0, 'sad': 0, 'bored': 0, 'neutral': 0, 'friendly': 0, 'happy': 0, 'excited': 0, 'joyful': 0}
sentiment_weights = {'angry': -1, 'annoyed': -0.8, 'frustrated': -0.6, 'sad': -0.4, 'bored': -0.2, 'neutral': 0.2, 'friendly': 0.4, 'happy': 0.6, 'excited': 0.8, 'joyful': 1}
        
@app.route("/")
def hello_world():
    return "<p>Hello, World!</p>"

def get_sentiment(sentiments_list, sentiments_weights):
    '''Returns the actual sentiment of the chatbot'''
    sentiment_sum = sum(sentiments_list.values())
  
    if sum(sentiments_list.values()) == 0:
        value = 0
    else:
        average = sum([sentiments_list[k]*sentiments_weights[k] for k, v in sentiments_list.items()]) / sum(sentiments_list.values())
        print(average)
        value = round(average * 5) / 5

    if value == 0: value = 0.2
    sentiment_weight_value = list(sentiments_weights.values()).index(value)

    if sentiment_weight_value == 0:
        return 'neutral'
    else:
        return list(sentiments_weights.keys())[sentiment_weight_value]    

def translate_sentiment(sentiment):
    if sentiment in ['angry','annoyed','frustrated']:
        return "Angry"
    elif sentiment in ['sad', 'bored']:
        return "Sad"
    elif sentiment in ['neutral', 'friendly']:
        return "Idle"
    elif sentiment in ['happy', 'excited', 'joyful']:
        return "Happy"
    else:
        return "Idle"

def return_sentimental_answer(input_sentence, sentiments_list, sentiments_weights):
    '''Function that returns the '''
    openai.api_key = api_key

    sentiment = openai.Completion.create(engine="text-davinci-002",
                                        prompt="Pick from these sentiments [angry, annoyed, frustrated, sad, bored, neutral, friendly, happy, excited, joyful]: " + input_sentence,
                                        temperature=0.4,
                                        max_tokens=100
                                        ).get("choices")[0].get("text").replace('\n', '').lower()

    if sentiment in sentiments_list.keys():
        sentiments_list[sentiment] += 1

    #Chatbot response
    chatbot_response = openai.Completion.create(
        engine="text-davinci-002",
        prompt=("Respond to: " + input_sentence),
        temperature=0.4,
        max_tokens=100
    ).get("choices")[0].get("text")

    #Chatbot emotional change
    emotional_response = openai.Completion.create(
        engine="text-davinci-002",
        prompt=" ".join(("Suggest a " + get_sentiment(sentiments_list, sentiment_weights) + " version of: " + chatbot_response).replace('\n', ' ').split()),
        temperature=0.6,
        max_tokens=100
    ).get("choices")[0].get("text")
    
    print('sentiment: ' +  sentiment + 'translate_sentiment: ' +  translate_sentiment(sentiment))

    resp = {"Message": emotional_response.replace('\n', ' '), "Emotion": translate_sentiment(sentiment)}
    
    return json.dumps(resp)
    


@app.route('/emhi', methods=["POST"])
def sentimental_chatbot():
    input_json = request.get_json(force=True)

    final_output = return_sentimental_answer(input_json['text'], 
                                            sentiments_list=sentiments_list, 
                                            sentiments_weights=sentiment_weights)

    return f"{final_output}"




if __name__ == '__main__':
    app.run(host='127.0.0.1', port='8500', debug=True)