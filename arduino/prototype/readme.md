# Prototype folder
This is the location of the main files used by each Arduino module, whether it be as a base station or drone.

## Commands
Commmand structure is simply `COMMAND RECEIVER SENDER DETAILS`, wherein:

- `COMMAND` - is a four-character string/char string that represents the command to be performed by the drone

- `RECEIVER` - is the name of the apparatus to perform the command

- `SENDER` - is the name of the apparatus that sent the command initially, and

- `DETAILS` - is extra strings/chars that may be used for additional processes. This can contain int/float values and such that require parsing

## Existing commands
| Command | Who can receive it | Who can send it | Details                                   | Example command in serial port | Purpose                                                                 |
|---------|--------------------|-----------------|-------------------------------------------|--------------------------------|-------------------------------------------------------------------------|
| GO      | DRO1, DRO2, DRO3   | BASE            | N/A                                       | `GO DRO3 YEET`                 | To resume drone moving if stopped initially                             |
| STOP    | DRO1, DRO2, DRO3   | BASE            | N/A                                       | `STOP DRO5 YEET`               | To halting drone deployment process                                     |
| TURN    | DRO1, DRO2, DRO3   | BASE            | int value (negative or positive integers) | `TURN DRO2 -50`                | To cause the drone to turn to a certain direction based on detail value |