import {themes as prismThemes} from 'prism-react-renderer';
import type {Config} from '@docusaurus/types';
import type * as Preset from '@docusaurus/preset-classic';

// This runs in Node.js - Don't use client-side code here (browser APIs, JSX...)

const config: Config = {
  title: 'Gump',
  favicon: 'img/favicon.ico',

  // Future flags, see https://docusaurus.io/docs/api/docusaurus-config#future
  future: {
    v4: true, // Improve compatibility with the upcoming Docusaurus v4
  },

  // Set the production url of your site here
  url: 'https://jeanbizeul.github.io',
  baseUrl: '/Gump/',

  // GitHub pages deployment config.
  organizationName: 'JeanBizeul',
  projectName: 'Gump',
  deploymentBranch: 'gh-pages',
  trailingSlash: true,

  onBrokenLinks: 'throw',
  onBrokenMarkdownLinks: 'throw',

  // Even if you don't use internationalization, you can use this field to set
  // useful metadata like html lang. For example, if your site is Chinese, you
  // may want to replace "en" with "zh-Hans".
  i18n: {
    defaultLocale: 'en',
    locales: ['en'],
  },

  presets: [
    [
      'classic',
      {
        docs: {
          sidebarPath: './sidebars.ts',
          routeBasePath: '/', // Serve the docs at the site's root
        },
        blog: false,
        theme: {
          customCss: './custom.css',
        },
      } satisfies Preset.Options,
    ],
  ],

  themeConfig: {
    navbar: {
      title: 'Gump Logo',
      logo: {
        alt: 'Gump Logo',
        src: 'img/favicon.ico',
      },
      items: [
        {
          type: 'docSidebar',
          sidebarId: 'gettingStartedSidebar',
          position: 'left',
          label: 'Getting Started',
        },
        {
          type: 'docSidebar',
          sidebarId: 'guidesSidebar',
          position: 'left',
          label: 'Guides',
        },
      ],
    },
    footer: {
      style: 'dark',
      links: [
        {
          title: 'Sections',
          items: [
            {
              label: 'Getting Started',
              to: '/',
            },
            {
              label: 'Guides',
              to: '/guides/setup',
            },
          ],
        },
        {
          title: 'More',
          items: [
            {
              label: 'GitHub',
              href: 'https://github.com/JeanBizeul/Gump',
            },
          ],
        },
      ],
      copyright: `Copyright © 2025 Grumpy, Inc. Built with Docusaurus.`,
    },
    prism: {
      theme: prismThemes.github,
      darkTheme: prismThemes.dracula,
    },
  } satisfies Preset.ThemeConfig,
};

export default config;