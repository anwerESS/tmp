@Override
public void doNotifyAutoCollat(final SettlementMessage settlementMessage) {

    // 1. Guard clauses instead of big nested if
    if (!autocollatEnabled) {
        return;
    }
    if (!CREATION.equals(useCase.from(settlementMessage))) {
        return;
    }

    final SettlementEvent<MxSese023> instruction = settlementMessage.getInstruction()
            .orElseThrow(logField(fieldName: "instruction"));

    final AutoCollateralDto autocollat = instruction.getMetadata().getAutocollat();
    if (autocollat == null) {
        return;
    }

    final String messageIdentifier = settlementMessage.getIdentifier();
    final String bookReference = settlementMessage.getBookReferenceInMetadata()
            .orElseThrow(logField(fieldName: "bookReference"));

    final Instant now = DateUtils.now();

    // 2. Use switch (or if/else) instead of 2 independent if
    handleAutocollatType(autocollat.getType(), messageIdentifier, bookReference, now);

    // 3. Put the creator logic in a dedicated method
    handleAutocollatCreator(autocollat.getCreator(), settlementMessage,
            messageIdentifier, bookReference, now);
}

private void handleAutocollatType(final AutoCollateralType type,
                                  final String messageIdentifier,
                                  final String bookReference,
                                  final Instant now) {

    if (type == null) {
        return;
    }

    switch (type) {
        case COLLATERAL_DELIVERY ->
                settlementDataExternalClient.saveEvent(
                        messageIdentifier,
                        bookReference,
                        EventCode.INS_LMA_COLLATERAL_DELIVERY,
                        now);

        case COLLATERAL_RELEASE ->
                settlementDataExternalClient.saveEvent(
                        messageIdentifier,
                        bookReference,
                        EventCode.INS_LMA_COLLATERAL_RELEASE,
                        now);

        default -> { /* nothing */ }
    }
}

private void handleAutocollatCreator(final AutoCollateralCreator creator,
                                     final SettlementMessage settlementMessage,
                                     final String messageIdentifier,
                                     final String bookReference,
                                     final Instant now) {

    if (creator != AutoCollateralCreator.ESM) {
        return;
    }

    settlementDataExternalClient.saveEvent(
            messageIdentifier,
            bookReference,
            EventCode.INS_AUTOCOLLATERAL_CREATE,
            now);

    autocollatClientService.notifyInstructionAutoCollatProcess(settlementMessage);
}