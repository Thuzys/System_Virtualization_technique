FROM node:alpine

ENV NODE_PORT=8004

EXPOSE 8004

USER node

WORKDIR /home/node/app

COPY --chown=node:node package.json .

COPY --chown=node:node files ./files

RUN npm install 

COPY --chown=node:node tvsapp.js .

CMD ["node", "tvsapp.js"]
