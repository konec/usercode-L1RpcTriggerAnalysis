<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="/">
    	<xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="pacdef">    
  	<html>
      <head>
        <title>PacDef</title>
      </head>
      <body>
      	<p>File generated <xsl:value-of select="date"/> </p>
      	<p><xsl:value-of select="descr"/> </p>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>
  
  <xsl:template match="date"></xsl:template>
  
  <xsl:template match="descr"></xsl:template>

 	<xsl:template match="qualitTable">
    <h3>Qualit table</h3>
    <table border="1" CELLPADDING="3">
      <xsl:apply-templates/>
    </table>
  </xsl:template>
  
  <xsl:template match="quality">
		<tr>  	
    	<td><xsl:value-of select="@id"/></td>
    	<td><xsl:value-of select="@planes"/></td> 
    	<td><xsl:value-of select="@val"/></td>
    </tr>
  </xsl:template>
  
  <xsl:template match="pac">    
  	<h3>Patterns for Tower: <xsl:value-of select="@tower"/> LogSector: <xsl:value-of select="@logSector"/> LogSegment: <xsl:value-of select="@logSegment"/></h3>
    <table border="1" CELLPADDING="3">
      <xsl:apply-templates/>
    </table>
  </xsl:template>
  
  <xsl:template match="pat">  
  	<tr>  
    	<td><xsl:value-of select="@num"/></td> 
    	<td><xsl:value-of select="@type"/>, <xsl:value-of select="@grp"/></td> 
    	<td><xsl:value-of select="@qual"/> </td> 
    	<xsl:apply-templates/>
    	<td><xsl:value-of select="@code"/></td>
    	<td><xsl:value-of select="@sign"/></td>
  	</tr>
  </xsl:template>

  <xsl:template match="str">      
  	<td align="center">
  	<xsl:if test="@f='99'">
    	<xsl:attribute name="style">background-color: #EAEAEA</xsl:attribute>
  	</xsl:if>  	
  	<xsl:value-of select="@f"/>-<xsl:value-of select="@t"/>	
  	</td>
  </xsl:template>
  
</xsl:stylesheet>
