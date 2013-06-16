<?xml version="1.0" encoding="UTF-8"?>

<!--
  interface.xslt

  Copyright 2013 Matthew Brush <mbrush@codebrainz.ca>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
  MA 02110-1301, USA.
-->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="yes" encoding="UTF-8"/>

<!-- Start -->
<xsl:template match="/">
  <xsl:comment>This file is auto-generated, do not edit.</xsl:comment>
  <geany>
    <xsl:attribute name="api">
      <xsl:value-of select="//memberdef[@kind='define' and name='GEANY_API_VERSION']/initializer"/>
    </xsl:attribute>
    <xsl:attribute name="abi">
      <!-- tranlsate hack needed since GEANY_ABI_VERSION has the gtk3 ABI shifting hack -->
      <xsl:value-of select="translate(//memberdef[@kind='define' and name='GEANY_ABI_VERSION']/initializer, translate(.,'0123456789',''), '')"/>
    </xsl:attribute>
    <macros>
      <xsl:apply-templates select="//compounddef[@kind='file']/sectiondef[@kind='define']/memberdef[@kind='define' and @prot='public']"/>
    </macros>
    <enumerations>
      <xsl:apply-templates select="//memberdef[@kind='enum' and @prot='public']"/>
    </enumerations>
    <structures>
      <xsl:apply-templates select="//compounddef[@kind='struct' and @prot='public']"/>
    </structures>
    <variables>
      <xsl:apply-templates select="//compounddef[@kind='file']/sectiondef[@kind='var']/memberdef[@kind='variable' and @prot='public' and not(contains(type, 'signal'))]"/>
    </variables>
    <functions>
      <xsl:apply-templates select="//memberdef[@kind='function' and @prot='public' and @static='no']"/>
    </functions>
    <signals>
      <xsl:apply-templates select="//compounddef[@kind='file']/sectiondef[@kind='var']/memberdef[@kind='variable' and @prot='public' and contains(type, 'signal')]"/>
    </signals>
  </geany>
</xsl:template>

<!-- Macros -->
<xsl:template match="//memberdef[@kind='define' and @prot='public']">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <macro>
      <name><xsl:value-of select="name"/></name>
      <initializer><xsl:value-of select="initializer"/></initializer>
      <brief><xsl:value-of select="briefdescription"/></brief>
    </macro>
  </xsl:if>
</xsl:template>

<!-- Enumerators -->
<xsl:template match="enumvalue[@prot='public']">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <enumerator>
      <name><xsl:value-of select="name"/></name>
      <xsl:if test="not(normalize-space(initializer)='')">
        <initializer><xsl:value-of select="initializer"/></initializer>
      </xsl:if>
      <brief><xsl:value-of select="briefdescription"/></brief>
    </enumerator>
  </xsl:if>
</xsl:template>

<!-- Enumerations -->
<xsl:template match="//memberdef[@kind='enum' and @prot='public']">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <enum>
      <name><xsl:value-of select="name"/></name>
      <brief><xsl:value-of select="briefdescription"/></brief>
      <enumerators>
        <xsl:apply-templates select="./enumvalue"/>
      </enumerators>
    </enum>
  </xsl:if>
</xsl:template>

<!-- Structure Members -->
<xsl:template match="sectiondef[@kind='public-attrib']/memberdef[@kind='variable']">
  <xsl:if test="not(normalize-space(briefdescription)='') and not(starts-with(type, 'signal'))">
    <member>
      <name><xsl:value-of select="name"/></name>
      <type><xsl:value-of select="type"/></type>
      <brief><xsl:value-of select="briefdescription"/></brief>
    </member>
  </xsl:if>
</xsl:template>

<!-- Structures -->
<xsl:template match="//compounddef[@kind='struct' and @prot='public']">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <structure>
      <name><xsl:value-of select="compoundname"/></name>
      <brief><xsl:value-of select="briefdescription"/></brief>
      <members>
        <xsl:apply-templates select="./sectiondef/memberdef"/>
      </members>
    </structure>
  </xsl:if>
</xsl:template>

<!-- Variables (globals) -->
<xsl:template match="compounddef[@kind='file']/sectiondef[@kind='var']/memberdef[@kind='variable' and @prot='public' and not(contains(type, 'signal'))]">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <variable>
      <name><xsl:value-of select="name"/></name>
      <type><xsl:value-of select="type"/></type>
      <brief><xsl:value-of select="briefdescription"/></brief>
    </variable>
  </xsl:if>
</xsl:template>

<!-- Function Parameter -->
<xsl:template match="param">
  <param>
    <name><xsl:value-of select="declname"/></name>
    <type><xsl:value-of select="type"/></type>
    <!-- <brief><xsl:value-of select="briefdescription"/></brief> -->
  </param>
</xsl:template>

<!-- Functions -->
<xsl:template match="//memberdef[@kind='function' and @prot='public' and @static='no']">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <function>
      <name><xsl:value-of select="name"/></name>
      <type><xsl:value-of select="type"/></type>
      <brief><xsl:value-of select="briefdescription"/></brief>
      <params><xsl:apply-templates select="./param"/></params>
    </function>
  </xsl:if>
</xsl:template>

<!-- Signals -->
<xsl:template match="compounddef[@kind='file']/sectiondef[@kind='var']/memberdef[@kind='variable' and @prot='public' and contains(type, 'signal')]">
  <xsl:if test="not(normalize-space(briefdescription)='')">
    <signal>
      <name><xsl:value-of select="name"/></name>
      <def><xsl:value-of select="definition"/></def>
      <brief><xsl:value-of select="briefdescription"/></brief>
    </signal>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
