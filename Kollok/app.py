from flask import Flask, request, jsonify, abort

app = Flask(__name__)

tasks = []
next_id = 1


@app.route("/tasks", methods=["GET"])
def get_tasks():
    return jsonify(tasks), 200


@app.route("/tasks", methods=["POST"])
def create_task():
    global next_id
    data = request.get_json()

    if not data or "title" not in data:
        abort(400)

    task = {
        "id": next_id,
        "title": data["title"],
        "description": data.get("description", ""),
        "status": data.get("status", "todo")
    }

    next_id += 1
    tasks.append(task)

    return jsonify(task), 201


@app.route("/tasks/<int:task_id>", methods=["GET"])
def get_task(task_id):
    task = next((t for t in tasks if t["id"] == task_id), None)
    if not task:
        abort(404)
    return jsonify(task), 200


@app.route("/tasks/<int:task_id>", methods=["PUT"])
def update_task(task_id):
    data = request.get_json()
    task = next((t for t in tasks if t["id"] == task_id), None)

    if not task:
        abort(404)

    task["title"] = data.get("title", task["title"])
    task["description"] = data.get("description", task["description"])
    task["status"] = data.get("status", task["status"])

    return jsonify(task), 200


@app.route("/tasks/<int:task_id>", methods=["PATCH"])
def patch_task(task_id):
    data = request.get_json()
    task = next((t for t in tasks if t["id"] == task_id), None)

    if not task:
        abort(404)

    if "status" in data:
        task["status"] = data["status"]

    return jsonify(task), 200


@app.route("/tasks/<int:task_id>", methods=["DELETE"])
def delete_task(task_id):
    global tasks
    task = next((t for t in tasks if t["id"] == task_id), None)

    if not task:
        abort(404)

    tasks = [t for t in tasks if t["id"] != task_id]
    return "", 204


if __name__ == "__main__":
    app.run(debug=True)
