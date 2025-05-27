from flask import Flask, render_template
import subprocess

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('index.html')


@app.route('/run-monitor')
def run_monitor():
    try:
        subprocess.run(['./page_fault_detection'], check=True, timeout=30)
        with open("page_fault_test_log.txt", "r") as file:
            content = file.read()
        return f"<pre>{content}</pre>"
    except Exception as e:
        return f"Error: {e}"


@app.route('/plot-monitor')
def plot_monitor():
    try:
        subprocess.run(['python3', 'plot_page_fault_detection.py'], check=True)
        return "Graph plotted successfully."
    except Exception as e:
        return f"Error: {e}"


@app.route('/run-solution')
def run_solution():
    try:
        subprocess.run(['./page_fault_sol'], check=True, timeout=30)
        with open("page_fault_solution_log.txt", "r") as file:
            content = file.read()
        return f"<pre>{content}</pre>"
    except Exception as e:
        return f"Error: {e}"


@app.route('/plot-solution')
def plot_solution():
    try:
        subprocess.run(['python3', 'plot_page_fault_sol.py'], check=True)
        return "Optimized graph plotted successfully."
    except Exception as e:
        return f"Error: {e}"


if __name__ == '__main__':
    app.run(debug=True)
