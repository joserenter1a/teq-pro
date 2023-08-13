import React, { useState } from 'react';
import './App.css';
import { VideoRoom } from './components/VideoRoom';
import CodeCompiler from '../../compiler/CodeCompiler';

function App() {
  const [joined, setJoined] = useState(false);

  const [code, setCode] = React.useState(
    `function add(a, b) {\n  return a + b;\n}`
  );
  return (
    <div className="App">
      <h1>TeqPro Virtual Call</h1>
      {!joined ? (
        <button onClick={() => setJoined(true)}>Join Room</button>
      ) : (
        <div className="container">
          <div className="video-container">
            <VideoRoom />
          </div>
          <div className="code-compiler-container">
            <CodeCompiler/>
          </div>
        </div>
      )}
    </div>
  );
}

export default App;