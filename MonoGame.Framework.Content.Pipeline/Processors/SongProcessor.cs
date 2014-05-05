// MonoGame - Copyright (C) The MonoGame Team
// This file is subject to the terms and conditions defined in
// file 'LICENSE.txt', which is part of this source code package.

using System;
using Microsoft.Xna.Framework.Content.Pipeline.Audio;
using System.IO;
using MonoGame.Framework.Content.Pipeline.Builder;

namespace Microsoft.Xna.Framework.Content.Pipeline.Processors
{
    /// <summary>
    /// A custom song processor that processes an intermediate AudioContent type. This type encapsulates the source audio content, producing a Song type that can be used in the game.
    /// </summary>
    [ContentProcessor(DisplayName = "Song - MonoGame")]
    public class SongProcessor : ContentProcessor<AudioContent, SongContent>
    {
        ConversionQuality quality = ConversionQuality.Best;

        /// <summary>
        /// Gets or sets the target format quality of the audio content.
        /// </summary>
        /// <value>The ConversionQuality of this audio data.</value>
        public ConversionQuality Quality { get { return quality; } set { quality = value; } }

        /// <summary>
        /// Gets or sets the desired format of the final audio content.
        /// </summary>
        public ConversionFormat? TargetFormat { get; set; }

        /// <summary>
        /// Initializes a new instance of SongProcessor.
        /// </summary>
        public SongProcessor()
        {
        }

        /// <summary>
        /// Builds the content for the source audio.
        /// </summary>
        /// <param name="input">The audio content to build.</param>
        /// <param name="context">Context for the specified processor.</param>
        /// <returns>The built audio.</returns>
        public override SongContent Process(AudioContent input, ContentProcessorContext context)
        {
            if (!TargetFormat.HasValue)
            {
                // Most platforms will use AAC ("mp4") by default
                TargetFormat = ConversionFormat.Aac;

                switch (context.TargetPlatform)
                {
                    case TargetPlatform.Windows:
                    case TargetPlatform.WindowsPhone8:
                    case TargetPlatform.WindowsStoreApp:
                        TargetFormat = ConversionFormat.WindowsMedia;
                        break;

                    case TargetPlatform.Linux:
                        TargetFormat = ConversionFormat.Vorbis;
                        break;

                    case TargetPlatform.PlayStation4:
                        TargetFormat = ConversionFormat.Atrac9;
                        break;
                }
            }

            // Get the song output path with the target format extension.
            var inputExtension = Path.GetExtension(input.FileName).TrimStart('.').ToLowerInvariant();

            // Allow PlayStation4 to override with an MP3 if it's provided instead of a WAV
            if (inputExtension == AudioHelper.GetExtension(ConversionFormat.Mp3) &&
                context.TargetPlatform == TargetPlatform.PlayStation4)
            {
                TargetFormat = ConversionFormat.Mp3;
            }

            var targetExtension = AudioHelper.GetExtension(TargetFormat.Value);
            var songFileName = Path.ChangeExtension(context.OutputFilename, targetExtension);

            // Make sure the output folder for the song exists.
            Directory.CreateDirectory(Path.GetDirectoryName(songFileName));

            if (inputExtension != targetExtension)
            {
                // Convert and write out the song media file.
                input.ConvertFormat(TargetFormat.Value, quality, songFileName);
            }
            else
            {
                // Pass the file through unmodified
                File.Copy(input.FileName, songFileName, true);
            }

            // Return the XNB song content.
            return new SongContent(PathHelper.GetRelativePath(Path.GetDirectoryName(context.OutputFilename) + Path.DirectorySeparatorChar, songFileName), input.Duration);
        }
    }
}
