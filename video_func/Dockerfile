# Use an official Node.js runtime as the base image
FROM node:14 AS build

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy package.json and package-lock.json to the container
COPY package*.json ./

# Install application dependencies
RUN NODE_ENV=development npm install

# Copy the rest of the application code to the container
COPY . .

# Build the Vite app
RUN npm run build

# Use a smaller base image for the runtime
FROM node:14

# Set the working directory for the runtime image
WORKDIR /usr/src/app

# Copy the build output from the build image to the runtime image
COPY --from=build /usr/src/app/build ./build

# Expose the port that your app is listening on
EXPOSE 5173

# Command to run your application
CMD ["npm", "start"]
