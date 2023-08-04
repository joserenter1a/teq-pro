import React, { useState } from 'react';
import axios from 'axios';

const languageOptions = [
  // All available languages
  { label: 'C#', value: '1' },
  { label: 'F#', value: '3' },
  { label: 'Java', value: '4' },
  { label: 'Python', value: '5' },
  { label: 'C (gcc)', value: '6' },
  { label: 'C++ (gcc)', value: '7' },
  { label: 'Php', value: '8' },
  { label: 'Haskell', value: '11' },
  { label: 'Ruby', value: '12' },
  { label: 'Perl', value: '13' },
  { label: 'Lua', value: '14' },
  { label: 'Nasm', value: '15' },
  { label: 'Javascript', value: '17' },
  { label: 'Go', value: '20' },
  { label: 'Scala', value: '21' },
  { label: 'D', value: '30' },
  { label: 'Swift', value: '37' },
  { label: 'Bash', value: '38' },
  { label: 'Erlang', value: '40' },
  { label: 'Elixir', value: '41' },
  { label: 'Ocaml', value: '42' },
  { label: 'Kotlin', value: '43' },
  { label: 'Rust', value: '46' },
  { label: 'Clojure', value: '47' },
  { label: 'ATS', value: '48' },
  { label: 'Cobol', value: '49' },
  { label: 'Coffeescript', value: '50' },
  { label: 'Crystal', value: '51' },
  { label: 'Elm', value: '52' },
  { label: 'Groovy', value: '53' },
  { label: 'Idris', value: '54' },
  { label: 'Julia', value: '55' },
  { label: 'Mercury', value: '56' },
  { label: 'Nim', value: '57' },
  { label: 'Nix', value: '58' },
  { label: 'Raku', value: '59' },
  { label: 'TypeScript', value: '60' },
];

function CodeCompiler() {
  const [output, setOutput] = useState('');
  const [isLoadingCompile, setIsLoadingCompile] = useState(false);
  const [isLoadingSubmit, setIsLoadingSubmit] = useState(false);
  const [code, setCode] = useState('');
  const [selectedLanguage, setSelectedLanguage] = useState('5'); // Default to Python (value 5)
  const [error, setError] = useState('');
  
  
  const handleRun = async () => {
    setIsLoadingCompile(true);
    setError(''); // Clear any previous errors

    const encodedParams = new URLSearchParams();
    encodedParams.set('LanguageChoice', selectedLanguage);
    encodedParams.set('Program', code);

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
      const result = response.data.Result;
      const errorOutput = response.data.Errors;

      if (errorOutput) {
        // If there is an error, combine it with the result and set it to output
        setOutput(errorOutput + '\n' + result);
        setError(errorOutput);
      } else {
        // If no error, set the result as the output
        setOutput(result);
        setError('');
      }
    } catch (error) {
      console.error(error);
      setError('Error compiling code. Please check your code and try again.');
    }

    setIsLoadingCompile(false);
  };

  
  const runTestCase = (code, input, expectedOutput) => {
    // Compare the actual output with the expected output
    const actualOutput = handleRun(code, input);
    return actualOutput.trim() === expectedOutput.trim();
  };

  const testCases = [
    {
      input: '5\n7\n',          // Example input for the first test case
      expectedOutput: '12\n',   // Example expected output for the first test case
    },
    {
      input: '10\n20\n',
      expectedOutput: '30\n',
    },
                                    // FIXME: Update test cases
  ];

  const handleSubmit = async () => {
    setIsLoadingSubmit(true);
    
    // // Initialize an array to store test results
    // const testResults = [];

    // // Run the code against each test case and collect the results
    // for (const testCase of testCases) {
    //   const { input, expectedOutput } = testCase;
    //   const testPassed = runTestCase(code, input, expectedOutput);
    //   testResults.push(testPassed ? 'Passed' : 'Failed');
    // }

    // // Join the test results and display them in the output area
    // setOutput(testResults.join('\n'));

    setIsLoadingSubmit(false);
  };

  return (
    <div style={{ padding:'20px' }}>
      <h1>Code Compiler</h1>
      <label>Enter Code here:</label>

      <br></br>
      <br />
      
      <div style={{ display: 'flex', gap: '10px' }}>    {/* Div around Text area */}
        {/* Text area for code input */}
        <textarea
          className="input"
          id="input"
          name="input"
          rows={20} // Number of visible text lines when needed
          style={{ 
            minHeight: '400px', 
            minWidth: '300px'
          }}
          onChange={(e) => setCode(e.target.value)}
        />

        {/* Text area for code output */}
        <textarea
          className="output"
          id="output"
          name="output"
          readOnly
          value={output}
          style={{
            minHeight: '400px',
            minWidth: '300px',
            color: error ? 'red' : 'black',
            backgroundColor: error ? '#ffebeb' : 'white',
          }}
        />  
      </div>

      <br></br><br></br>

      {/* Programming Language Selector */}
      <label>Choose a programming language:</label>
      <select value={selectedLanguage} onChange={(e) => setSelectedLanguage(e.target.value)}>
        {languageOptions.map((option) => (
          <option key={option.value} value={option.value}>
            {option.label}
          </option>
        ))}
      </select>

      <br></br><br></br>

      <div>
        <center>
          {/* Compilation Button */}
          <button 
            type="button" 
            class="CompileSubmitButton" 
            onClick={handleRun} 
            disabled={isLoadingCompile}>
            {isLoadingCompile ? 'Compiling...' : 'Compile'}
          </button>

          {/* Submit Button */}
          <button 
            type="button" 
            class="CompileSubmitButton" 
            onClick={handleSubmit} 
            disabled={isLoadingSubmit}>
            {isLoadingSubmit ? 'Submitting...' : 'Submit'}
          </button>
        </center>
      </div>
    </div>
  );
}
export default CodeCompiler;