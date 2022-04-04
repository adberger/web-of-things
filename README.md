# Web of Things for BTI4105 - DSIOT Systems

## Basic idea
Implement a Web of Things gateway to connect multiple protocols (LoRa, ZigBee, ...) with each other. 

## Project structure
| Directory     | Description                                                                                                              |
|---------------|--------------------------------------------------------------------------------------------------------------------------|
| `ttn-adapter` | A fork from the [`ttn-adapter`](https://github.com/tim-hellhake/ttn-adapter) with added support for TheThingsNetwork V3. |
| `wot-gateway` | Docker Compose for starting a local Web Of Things Gateway (using WebThings).                                             |

## Local development
### Develop `ttn-adapter`
- Install dependencies using `npm install`
- Implement the changes in the `ttn-adapter` directory
- Build the project using `npm run build`
- Package the app using `./package.sh` (needs a linux shell such as `Git Bash`)
- Copy the contents of the `package/` directory of the newly generated `ttn-adapter-v3-x.x.x.tgz` to `wot-gateway/shared/addons/ttn-adapter-v3`
- Restart the WebThings Gateway Docker container

### Start WebThings Gateway
- Run `docker-compose up -d` in the `wot-gateway` directory

After changes the WebThings Gateway needs to be restarted.
