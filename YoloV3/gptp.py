import openai
import time
from gtts import gTTS
import os
import speech_recognition as sr

openai.api_key = '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'

def get_chat_model_response(messages):
    response = openai.ChatCompletion.create(
      model="gpt-3.5-turbo",
      messages=messages
    )
    return response['choices'][0]['message']['content']

messages = [
    {"role": "system", "content": "You are a helpful assistant."},
    {"role": "user", "content": "Who won the world series in 2020?"},
]

# Initialize recognizer
r = sr.Recognizer()

while True:
    with sr.Microphone() as source:
        print("Speak Anything :")
        audio = r.listen(source, timeout=1)  # Timeout set to 2 seconds
        try:
            user_prompt = r.recognize_google(audio)
            print("You said : {}".format(user_prompt))
        except:
            print("Sorry could not recognize your voice")
            continue  # Skip this iteration and go to the next one

    if user_prompt.lower() == 'quit':
        break

    messages.append({"role": "user", "content": user_prompt})
    ai_response = get_chat_model_response(messages)
    print("AI: ", ai_response)

    tts = gTTS(text=ai_response, lang='en')
    tts.save("speech.mp3")
    # Use the command line to play the generated mp3
    os.system("mpg321 speech.mp3")
    time.sleep(1)  # wait for 1 second to respect the rate limits
