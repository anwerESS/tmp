@Override
public void doNotifyAutoCollat(final SettlementMessage settlementMessage) {

    if (!autocollatEnabled) {
        return;
    }
    if (!CREATION.equals(useCase.from(settlementMessage))) {
        return;
    }

    final SettlementEvent<MxSese023> instruction = settlementMessage.getInstruction()
            .orElseThrow(logField("instruction"));

    final AutoCollateralDto autocollat = instruction.getMetadata().getAutocollat();
    if (autocollat == null) {
        return;
    }

    final String messageIdentifier = settlementMessage.getIdentifier();
    final String bookReference = settlementMessage.getBookReferenceInMetadata()
            .orElseThrow(logField("bookReference"));

    final AutoCollateralCreator creator = autocollat.getCreator();
    final AutoCollateralType type = autocollat.getType();
    final Instant now = DateUtils.now();

    if (type == AutoCollateralType.COLLATERAL_DELIVERY) {
        saveEvent(messageIdentifier, bookReference,
                EventCode.INS_LMA_COLLATERAL_DELIVERY, now);
    } else if (type == AutoCollateralType.COLLATERAL_RELEASE) {
        saveEvent(messageIdentifier, bookReference,
                EventCode.INS_LMA_COLLATERAL_RELEASE, now);
    }

    if (creator == AutoCollateralCreator.ESM) {
        saveEvent(messageIdentifier, bookReference,
                EventCode.INS_AUTOCOLLATERAL_CREATE, now);
        autocollatClientService.notifyInstructionAutoCollatProcess(settlementMessage);
    }
}

private void saveEvent(String messageIdentifier,
                       String bookReference,
                       EventCode eventCode,
                       Instant timestamp) {

    settlementDataExternalClient.saveEvent(messageIdentifier, bookReference, eventCode, timestamp);
}