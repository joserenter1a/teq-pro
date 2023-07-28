const express = require('express')
const app = express()
const port = process.env.PORT || 8000

app.use(express.json());

// app.get("/api", (req, res) => {
//     res.json({"users": ["userOne", "userTwo", "userThree", "userFour", "userFive"]});
// });

app.listen(port, () => {
    console.log(`Server started on port http://localhost:${port}`);
});


const axios = require('axios');

async function makeCompilerRequest() {
  const encodedParams = new URLSearchParams();
  encodedParams.set('LanguageChoice', '5');
  encodedParams.set('Program', 'print("Hello World!, on python language")');

  const options = {
    method: 'POST',
    url: 'https://code-compiler.p.rapidapi.com/v2',
    headers: {
      'content-type': 'application/x-www-form-urlencoded',
      'X-RapidAPI-Key': 'c40e63ca05msh21cc53be5c61ed5p1be771jsnda85c9acad28',
      'X-RapidAPI-Host': 'code-compiler.p.rapidapi.com'
    },
    data: encodedParams,
  };

  try {
    const response = await axios.request(options);
    console.log(response.data);
  } catch (error) {
    console.error(error);
  }
}

// Call the function to make the compiler request
makeCompilerRequest();