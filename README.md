<img align="right" src="https://cdn-icons-png.flaticon.com/512/6124/6124995.png" width="200">

# SoBay
Auction System for Linux, with a server/client architecture.
<br><br>
## Features 📑
* Backend: The application responsible for responding to all the requests from the clients
* Frontend: The application where all clients login to participate in the action
> No Frontend can be initialized without the Backend being ON
<br><br>
## How to Use 🛠
In the terminal use the following command: 
<br>
`make rr` - This will initialize/compile all the programms needed
<br><br>
`./backend` - to initialize the backend application
<br>
`./frontend <username> <password>` - to initialize the frontend application
<br><br>
## Commands ⚙ (Backend)
* `users` : prints all the users currently online
* `kick <username>` : kicks the user
* `list` : lists all the items for sale
* `prom` : list all the promotions active
* `reprom` : reload the promotions programs based on the txt file
* `cancel` : close the exe of one of the promotors program
* `help` : prints a command list
* `close` :  closes the backend and all the frontends
