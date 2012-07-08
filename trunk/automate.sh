#!/bin/bash

userid="qfransse"
mail="$userid@backus.info.fundp.ac.be"
mail2="$userid@sunset.info.fundp.ac.be"
url="12345:concours.info.fundp.ac.be:80"

ssh -L $url $mail2

#ssh -L 12345:concours.info.fundp.ac.be:80 qfransse@backus.info.fundp.ac.be

echo "Launch browser : http://localhost:12345/automate" 
