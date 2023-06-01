/*
 ********************* Html.cpp *********************
 * This file embeds the html file in C++ code.	 	*
 ****************************************************
 */

#pragma once

#include <iostream>
#include <string>

extern const char *Html =
        "<!DOCTYPE html>\n"
        "<html lang=\"en\">\n"
        "\n"
        "<head>\n"
        "\t<meta charset=\"UTF-8\">\n"
        "\t<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
        "\t<meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">\n"
        "\t<title>AST_Visualization</title>\n"
        "\t<script src=\"http://d3js.org/d3.v3.min.js\" charset=\"utf-8\"></script>\n"
        "\t<style>\n"
        "\t\t.node circle {\n"
        "\t\t\tfill: #fff;\n"
        "\t\t\tstroke: black;\n"
        "\t\t\tstroke-width: 1.0px;\n"
        "\t\t}\n"
        "\n"
        "\t\t.node text {\n"
        "\t\t\tfont-size: 5px;\n"
        "\t\t\tcolor: black;\n"
        "\t\t}\n"
        "\n"
        "\t\t.link {\n"
        "\t\t\tfill: none;\n"
        "\t\t\tstroke: black;\n"
        "\t\t\tstroke-width: 1.0px;\n"
        "\t\t}\n"
        "\t</style>\n"
        "</head>\n"
        "\n"
        "<body>\n"
        "\t<script>\n"
        "\t\tvar width = 3000;\n"
        "\t\tvar height = 3000;\n"
        "\t\tvar svg = d3.select(\'body\')\n"
        "\t\t\t.append(\'svg\')\n"
        "\t\t\t.attr(\'width\', width)\n"
        "\t\t\t.attr(\'height\', height)\n"
        "\t\t\t.append(\'g\')\n"
        "\t\t\t.attr(\'transform\', \'translate(50,0)\')\n"
        "\t\tvar tree = d3.layout.tree()\n"
        "\t\t\t.size([width, height - 2000])\n"
        "\n"
        "\t\tvar diagonal = d3.svg.diagonal()\n"
        "\t\t\t.projection(d => [d.y, d.x])\n"
        "\t\tvar root = ${ASTJson};\n"
        "\t\tvar nodes = tree.nodes(root)\n"
        "\t\tvar links = tree.links(nodes)\n"
        "\n"
        "\t\tvar maxNodeSize = d3.max(nodes, function(d) { return d.size; });\n"
        "\t\tvar nodeRadiusScale = d3.scale.sqrt().domain([0, maxNodeSize]).range([0, 50]);\n"
        "\t\tvar link = svg.selectAll(\'.link\')\n"
        "\t\t\t.data(links)\n"
        "\t\t\t.enter()\n"
        "\t\t\t.append(\'path\')\n"
        "\t\t\t.attr(\'class\', \'link\')\n"
        "\t\t\t.attr(\'d\', diagonal)\n"
        "\n"
        "\t\tvar node = svg.selectAll(\'.node\')\n"
        "\t\t\t.data(nodes)\n"
        "\t\t\t.enter()\n"
        "\t\t\t.append(\'g\')\n"
        "\t\t\t.attr(\'class\', \'node\')\n"
        "\t\t\t.attr(\'transform\', function (d) {\n"
        "\t\t\t\treturn \"translate(\" + d.y + \",\" + d.x + \")\";\n"
        "\t\t\t});\n"
        "\n"
        "\t\tnode.append(\'circle\')\n"
        "\t\t\t.attr(\"r\", function(d) { return Math.sqrt(d.size) / Math.PI; })\n"
        "\t\t\t.style('fill', '#ffffff')\n"
        "\t\t\t.style('stroke', '#000000')\n"
        "\t\t\t.style('stroke-width', '1px');\n"
        "\n"
        "\n"
        "\t\tnode.append(\"text\")\n"
        "\t\t\t.attr(\"dx\", function (d) { return d.children ? -8 : 8; })\n"
        "\t\t\t.attr(\"dy\", 5)\n"
        "\t\t\t.attr(\"text-anchor\", function(d) { return d.children ? \"end\" : \"start\"; })\n"
        "\t\t\t.attr(\'class\', \'text\')\n"
        "\t\t\t.style(\"font-size\", \"12px\")\n"
        "\t\t\t.text(function (d) { return d.name; });\n"
        "\t</script>\n"
        "</body>\n"
        "\n"
        "</html>\n";