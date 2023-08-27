package edu.msoe.sefocus.pcgui;

import javax.swing.*;
import java.awt.*;

public class DiagnosticControls extends JPanel {
    private JCheckBox printNewHeaders = new JCheckBox("Print New Frame Headers");
    private JCheckBox extraDebugInfo = new JCheckBox("Print Extra Debug Info");
    private JCheckBox printFulHexRecptionLog = new JCheckBox("Print Hex reception data");

    public DiagnosticControls()
    {
        this.setLayout(new FlowLayout());

        this.add(printNewHeaders);
        this.add(extraDebugInfo);
        this.add(printFulHexRecptionLog);
    }

    public boolean isPrintNewHeadersActive()
    {
        return printNewHeaders.isSelected();
    }

    public boolean isExtraDebugInfoActive()
    {
        return extraDebugInfo.isSelected();
    }

    public boolean isPrintHexLogEnabled()
    {
        return printFulHexRecptionLog.isSelected();
    }

}
