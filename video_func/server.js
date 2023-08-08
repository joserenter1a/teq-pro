import express from 'express'
import response  from 'express/lib/response.js';
const app = express();
import http from 'http';
const server = http.createServer(app);
import { Server } from "socket.io";
const io = new Server(server);
import path from 'path'

const PORT = 5172
const ADDRESS = `localhost`;

app.get('/home', (request, response) => {
    response 
        .status(200)
        .setHeader('Content-type', 'text/html')
        .sendFile(path.join(__dirname, 'index.html'));
    });

app.get('/chat', (request, response) => {
  response
    .status(200)
    .setHeader('Content-type', 'text/html')
    .sendFile(__dirname + 'socket.html');
});

io.on('connection', (socket) => {
    console.log('a user connected');
    socket.on('disconnect', () => {
        console.log('user disconnected');
    });
});

io.on('connection', (socket) => {
    socket.on('chat message', (msg) => {
        console.log(`message: ${msg}`);
    });
});

io.emit('some event', { someProperty: 'some value', otherProperty: 'other value' }); // This will emit the event to all connected sockets

io.on('connection', (socket) => {
    socket.broadcast.emit('hi');
})

io.on('connection', (socket) => {
    socket.on('chat message', (msg) => {
        io.emit('chat message', msg);
    });
});

server.listen(PORT, () => {
    console.log(`Server is running on port http://${ADDRESS}:${PORT}`);
  });