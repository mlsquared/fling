# Responsive Song/Video Queue

**Nicolantonio Prentosito, Larson Carter, Koichi Nishida**  
Sponsored by **Professor David Harrison**  

---

This project’s objective is to develop a webpage interface that displays a queue of songs/videos to be played, and plays them. The queue will be populated by a “preferred” song/video of a present, known individual, who is identified as present by connecting to the same Wi-Fi network as the application. As a known user joins the network, their song/video will be added to the queue, and if they leave the network before it plays, then it will be removed. “Preferred songs/videos” are stored in a database of known users. This project is the idea of Professor Harrison and is related with some of his research work, and was selected due to his recommendation.

The MVP for this project consists of a functional User Interface that will update a queue of songs/videos based on who is present (as determined by connection to the same Wi-Fi network). If a person attaches to the Wi-Fi network, their “preferred” song/video will be added to the queue, and as someone leaves the network, their song/video is removed from the queue.

One challenging component of this project is determining whether a device is attached to the same Wi-Fi network. Two more challenging components are learning new framework(s) for designing the user interface and automating the task of adding and deleting from the queue.

The users of this product are anyone looking for entertainment from music or videos, wanting personalized songs/videos to play based on who is present. As a recreational user, I would want my song/video to be automatically added to the queue when I connect to the Wi-Fi network, so that I can avoid any unnecessary hassle. As a recreational user, I would want to be able to see which songs are in the queue, so that I know what is coming up. As a recreational user, I want my song/video removed from the queue automatically if I disconnect from the Wi-Fi network, so that the songs/videos played are only the ones preferred by those present.

The tasks that make up this project include learning to use a Raspberry Pi and output from it to a TV, selecting a Framework for the API and learning how to use it, completing a database to store the list of video/song IDs associated with user IDs, building a queue application, and developing the queue to update automatically based on present users.

### Tentative Timeline:
- **1 week** to produce a “Hello World” on the Raspberry Pi with output to a TV via HDMI.
- **2 weeks** to build simple dialog boxes in 2-3 frameworks, try them out, output “Hello Worlds”, and select the best one.
- **1 week** to set up database mapping of user IDs to a list of song/video IDs, with command line ability to add, remove, and list songs/videos for users.
- **3 weeks** to produce a rough but functional queue app for TV: UI that shows a queue of videos/songs on Raspberry Pi along with a command line tool for manipulating the queue.
- Finally, at least **1 week before the final presentation**, the MVP should be complete: a functional UI that updates the queue to automatically add or remove videos as users become present or absent (determined by Wi-Fi connection).


### Objectives, Technical Considerations, and Challenges

1. **Wi-Fi User Detection**  
   - **Challenge**: Identifying when known users join or leave the Wi-Fi network.  
   - **Considerations**: 
     - How to access Wi-Fi connection logs and monitor connection securely and efficiently.
     - Ensuring accuracy in detecting when a device connects or disconnects from the network.
     - Privacy concerns and preventing unauthorized access to user data.

2. **Database Integration**  
   - **Challenge**: Designing  a database that will map user identities to their preferred songs/videos.  
   - **Considerations**:
     - Choosing SQL nor NoSQL databases for scalability and ease of use.
     - Data security for storing user information and preferences.
     - Efficient queries to update and retrieve user song/video preferences in real-time.

3. **Queue Automation**  
   - **Challenge**: Automating the addition and removal of songs/videos from the queue in real-time as users connect or disconnect from the network.  
   - **Considerations**:
     - Handling real-time updates with minimal delays.
     - Handling synchronization issues when multiple users join or leave the network simultaneously.
     - Avoiding conflicts like duplicate songs or errors when a user disconnects before their song plays.

4. **User Interface Development**  
   - **Challenge**: Developing a responsive and user-friendly UI that reflects the current state of the song/video queue.  
   - **Considerations**:
     - Selecting the right front-end framework that supports real-time updates.
     - Ensuring the UI is intuitive and displays appropriate and accurate information about the current queue and connected users.
     - Managing the user experience across different device types, including TVs and web browsers.

5. **Raspberry Pi Integration**  
   - **Challenge**: Displaying the queue and managing song playback using a Raspberry Pi connected to a TV.  
   - **Considerations**:
     - Optimizing performance for media playback on a Raspberry Pi.
     - Ensuring reliable communication between the Raspberry Pi and the backend system for real-time queue updates.

6. **Scalability and Performance**  
   - **Challenge**: Ensuring the system scales well as more users connect or the number of preferred songs/videos increases.  
   - **Considerations**:
     - Efficiently managing resources (e.g., memory, processing power) on the Raspberry Pi and the web server.
     - Load balancing and performance testing for multiple simultaneous users.
     - Handling large queues without poor performance or lag in playback.

7. **Security and Privacy**  
   - **Challenge**: Protecting user data and ensuring that the system adheres to privacy best practices.  
   - **Considerations**:
     - Ensuring secure handling of Wi-Fi connection data to prevent unauthorized access.
     - Encrypting user preferences stored in the database.
     - Minimizing data collection to only what's necessary for system functionality.


### Process Flow Chart for User Activity:

![Process Flow Chart for User Activity](flow_chart.png)
