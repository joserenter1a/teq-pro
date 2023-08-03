import express from 'express'

const PORT = 5173
const ADDRESS = `localhost`
const app = express();

app.get('/home', (request, response) => {
    response 
        .status(200)
        .setHeader('Content-type', 'text/html')
        .sendFile(path.join(__dirname, 'index.html'));
    });


app.listen(PORT, () => {
  console.log(`Server is running on port http://${ADDRESS}:${PORT}`);
});