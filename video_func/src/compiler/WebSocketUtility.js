
const createWebSocketConnection = () => {
    const ws = new WebSocket('ws://your-websocket-server-url');
  
    ws.onopen = () => {
      console.log('WebSocket connection established.');
    };
  
    ws.onmessage = (event) => {
      // Handle received data here
      const data = JSON.parse(event.data);
      // Update code in CodeCompiler component based on the received data
      // For example: setCode(data.code);
    };
  
    ws.onclose = () => {
      console.log('WebSocket connection closed.');
    };
  
    ws.onerror = (error) => {
      console.error('WebSocket error:', error);
    };
  
    return ws;
  };
  
  export default createWebSocketConnection;