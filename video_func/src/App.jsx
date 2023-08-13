import React, { useState } from 'react';
import './App.css';
import { VideoRoom } from './components/VideoRoom';
import CodeCompiler from '../../compiler/CodeCompiler';

function App() {
  const [joined, setJoined] = useState(false);
  const [navbarOpen, setNavbarOpen] = useState(false);
  const [text, setText] = useState('Input Room Code Here');

  const [code, setCode] = React.useState(
    `function add(a, b) {\n  return a + b;\n}`
  );
  return (
    <div className="App">
      <h1>TeqPro Virtual Call</h1>
      {!joined ? (
        <div>
          <nav className='navbar'>
            <button className='hamb' onClick={()=>setNavbarOpen((prev)=>!prev)}>
              {navbarOpen ?
              <ul>
                Settings
                <li className='hambel'>
                  <button>Help</button>
                </li>
                <li className='hambel'>
                  <button>HowTo</button>
                </li>
                </ul> 
              :'Settings'}
            </button>
          </nav>
        <input type="text" className='room-input' value = {text}
        onClick={e=> setText('')}
        onChange={e=> setText(e.target.value)}
        />
        <button onClick={() => setJoined(true)}>Join Room</button>
        </div>
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