const express = require("express");
const http = require("http");
const { Server } = require("socket.io");
const path = require("path");

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// client.html ko serve karna
app.use(express.static(path.join(__dirname)));

io.on("connection", (socket) => {
  console.log("Client connected:", socket.id);

  socket.on("message", (msg) => {
    console.log("Received:", msg);
    socket.emit("message", `Server received: ${msg}`);
  });

  socket.on("disconnect", () => {
    console.log("Client disconnected:", socket.id);
  });
});

server.listen(3000, () => {
  console.log("✅ Server running on http://localhost:3000");
});
