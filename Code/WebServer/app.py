from flask import Flask, render_template, request, redirect, flash, url_for, jsonify
import json

app = Flask(__name__)
app.secret_key = "Daddy Dec on the beat"

colorCode= '#ff1122'
threeColors = ['#111111','#222222','#333333']
sevenColors = ['#111111','#222222','#333333','#111111','#222222','#333333','#111111']

pattern='white'

mode='single'
color = '#123456'


#server side rendering 
@app.route("/")
def index():
    return render_template('test.html', colorCode=colorCode, pattern=pattern)

#server side rendering
@app.route("/form", methods=["POST"])
def form():
    global colorCode 
    global pattern
    colorCode = request.form.get('color') #html input name parameter
    pattern = request.form.get('pattern')
    flash('Colour Changed!', category="success")
    return redirect('/')

#server side rendering
@app.route("/query")
def query():
    dict = {}

    global pattern
    global colorCode
    global threeColors

    for variable in ["pattern"]:
        dict[variable] = eval(variable)
    return dict 


@app.route("/pattern")
def colors():
    dict = {}
    global pattern
    for variable in ["pattern"]:
        dict[variable] = eval(variable)
    return dict 
