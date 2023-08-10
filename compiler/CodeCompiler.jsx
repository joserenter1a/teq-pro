import React, { useState, useEffect } from 'react';
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
        setOutput(errorOutput + '\n' + result);
        setError(errorOutput);
      } else {
        setOutput(result);
        setError('');
      }
    } catch (error) {
      console.error(error);
      setError('Error compiling code. Please check your code and try again.');
    }

    setIsLoadingCompile(false);
  };

  const testCases = [
      {
  functionCode: `
    def fizz_buzz(n):
        result = []
        for i in range(1, n+1):
            if i % 3 == 0 and i % 5 == 0:
                result.append("FizzBuzz")
            elif i % 3 == 0:
                result.append("Fizz")
            elif i % 5 == 0:
                result.append("Buzz")
            else:
                result.append(str(i))
        return '\n'.join(result)
  `,
expectedOutput: `1
2
Fizz
4
Buzz
Fizz
7
8
Fizz
Buzz
11
Fizz
13
14
FizzBuzz
16
17
Fizz
19
Buzz
Fizz
22
23
Fizz
Buzz
26
Fizz
28
29
FizzBuzz
31
32
Fizz
34
Buzz
Fizz
37
38
Fizz
Buzz
41
Fizz
43
44
FizzBuzz
46
47
Fizz
49
Buzz
`},]

  const runTestCase = async (userCode, expectedOutput) => {
    setIsLoadingCompile(true);
    setError('');
  
    const encodedParams = new URLSearchParams();
    encodedParams.set('LanguageChoice', selectedLanguage);
    encodedParams.set('Program', userCode);
  
    const options = {
      method: 'POST',
      url: 'https://code-compiler.p.rapidapi.com/v2',
      headers: {
        'content-type': 'application/x-www-form-urlencoded',
        'X-RapidAPI-Key': 'c40e63ca05msh21cc53be5c61ed5p1be771jsnda85c9acad28',
        'X-RapidAPI-Host': 'code-compiler.p.rapidapi.com',
      },
      data: encodedParams,
    };
  
    try {
      const response = await axios.request(options);
      const result = response.data.Result;
      const errorOutput = response.data.Errors;
  
      setIsLoadingCompile(false);
  
      if (errorOutput) {
        setError(errorOutput);
        return 'Test Failed!';
      } else if (result === expectedOutput) {
        return 'Test Passed!';
      } else {
        return 'Test Failed!';
      }
    } catch (error) {
      console.error(error);
      setError('Error compiling code. Please check your code and try again.');
      setIsLoadingCompile(false);
      return 'Test Failed!';
    }
  };

  const handleSubmit = async () => {
    setIsLoadingSubmit(true);
    const testCase = testCases[0];
    const userCode = code;
    const testResult = await runTestCase(userCode, testCase.expectedOutput);

    if (testResult === 'Test Failed!') {
      setError(true);
      const finalOutput = `${testResult}\n\nExpected Output:\n${testCase.expectedOutput}`;
      setOutput(finalOutput); 
      setIsLoadingSubmit(false);
    } else {
      const finalOutput = `${testResult}\n\nYour Output:\n${testCase.expectedOutput}`;
      setOutput(finalOutput); 
      setIsLoadingSubmit(false);
    }
  };

  return (
    <div>
      <h2 className="compiler-title">Code Compiler</h2>

      <div style={{ display: 'flex', gap: '10px' }}>  {/* Div around Text area */}
        <textarea
          className="input"
          id="input"
          name="input"
          rows={20}
          onChange={(e) => setCode(e.target.value)}
          value={code} // Set the value of the textarea to the state "code"
        />

        {/* Text area for code output */}
        <textarea
          className="output"
          id="output"
          name="output"
          readOnly
          value={output}
          style={{
            color: error ? 'red' : 'black',
            backgroundColor: error ? '#ffebeb' : 'white',
          }}
          />
      </div>

      <div className="language-selector">
        {/* Programming Language Selector */}
        <label>Choose a programming language:</label>
        <select value={selectedLanguage} onChange={(e) => setSelectedLanguage(e.target.value)}>
          {languageOptions.map((option) => (
            <option key={option.value} value={option.value}>
              {option.label}
            </option>
          ))}
        </select>
      </div>

      <div className="language-buttons-container">
        <center>
          {/* Compilation Button */}
          <button 
            type="button" 
            className="CompileSubmitButton" 
            onClick={handleRun} 
            disabled={isLoadingCompile}>
            {isLoadingCompile ? 'Compiling...' : 'Compile'}
          </button>

          {/* Submit Button */}
          <button 
            type="button" 
            className="CompileSubmitButton" 
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

