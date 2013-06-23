#!/usr/bin/env python2.7

import xml.etree.ElementTree as ET
from xml.dom import minidom

import argparse, os, sys, gzip

def prettify(elem):
   """
Return a pretty-printed XML string for the Element.
"""
   rough_string = ET.tostring(elem, 'utf-8').replace('\n', '')
   reparsed = minidom.parseString(rough_string)
   return reparsed.toprettyxml(indent=" ")

def parse_manifest(xml, excludelist):
   '''
Parse the manifest and return a list with name of project to exclude
'''
   tree = ET.parse(xml)
   root = tree.getroot()
   names = []
   for child in root:
      if child.tag == 'project':
         path = child.attrib['path'].split('/')
         for to_exclude in excludelist:
            if to_exclude in path:
               names.append(child.attrib['name'])
            else:
               pass
   return names

def add_to_local_manifest(excluded, local_xml = None):
   '''
Add the tags remove-project with all the project to remove to
an existing local_manifest.xml
'''
   if local_xml:
      tree = ET.parse(local_xml)
      root = tree.getroot()
   if local_xml:
      new_root = ET.Element(root.tag)
   else:
      new_root = ET.Element('manifest')
   for excluded_i in excluded:
      new_root.append(ET.Element("remove-project", {"name" : excluded_i}))
   if local_xml:
      for child in root:
         new_root.append(child)
   return new_root

def main():
   '''
Execute the function with args
'''
   parser = argparse.ArgumentParser(description='Help to create a local_manifest.xml to removes the projects matching defines keywords from an android manifest.xml')
   parser.add_argument('manifest', metavar = 'manifest',
                   help='The default manifest.xml.')
   parser.add_argument('-l', '--local', dest = 'local', default = None,
                   help='A local manifest seed to add the remove-projects tag')
   parser.add_argument('-x', '--excluded', dest = 'excluded', type = str, default = "acer,asus,bn,htc,lge,motorola,oppo,samsung,sony",
                   help='A list of name (brands) comma separated that we want to exclude from the main manifest.xml. default "acer,asus,bn,htc,lge,motorola,oppo,samsung,sony"')

   args = parser.parse_args()
   excluded = args.excluded.split(',')
   project_names = parse_manifest(args.manifest, excluded)
   manifest_removed = add_to_local_manifest(project_names, local_xml = args.local)
   #manifest_removed.write(sys.stdout, encoding='utf-8')
   print prettify(manifest_removed)

if __name__ == "__main__":
   main()