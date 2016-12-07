
#include "PDS.hpp"
#include "Tools.hpp"
#include <density/ScalePyramid.hpp>

namespace pds
{

	std::vector<Eigen::Vector2f> FloydSteinberg(const Eigen::MatrixXf& density_inp)
	{
		Eigen::MatrixXf density = density_inp;
		std::vector<Eigen::Vector2f> seeds;
		for(unsigned int y=0; y<density.cols() - 1; y++) {
			density(1,y) += density(0,y);
			for(unsigned int x=1; x<density.rows() - 1; x++) {
				float v = density(x,y);
				if(v >= 0.5f) {
					v -= 1.0f;
					seeds.push_back(
						Eigen::Vector2f{
							static_cast<float>(x) + 0.5f,
							static_cast<float>(y) + 0.5f
						});
				}
				density(x+1,y  ) += 7.0f / 16.0f * v;
				density(x-1,y+1) += 3.0f / 16.0f * v;
				density(x  ,y+1) += 5.0f / 16.0f * v;
				density(x+1,y+1) += 1.0f / 16.0f * v;
			}
			// carry over
			density(0, y+1) += density(density.rows()-1, y);
		}
		return seeds;
	}

	// Variante von Floyd-Steinberg. Vorteil: Keine Schlangenlinien in dünn besetzten Bereichen.
	std::vector<Eigen::Vector2f> FloydSteinbergExpo(const Eigen::MatrixXf& density)
	{
		// Fehler der nächsten 8 Zeilen in Ringpuffer speichern
		Eigen::MatrixXf ringbuffer( 16 + density.rows(), 8 );
		ringbuffer.fill( {0.0f} );

		// Eine schnelle Zufallszahl
		unsigned int crc32 = 0xffffffff;

		// Bild abtasten
		std::vector<Eigen::Vector2f> seeds;
		for(unsigned int y=0; y < density.cols(); y++)
		{
			float *pRingBuf = &ringbuffer( 8, y % 8 );
			unsigned int x = 0;
			while( x < density.rows() )
			{
				// Dichte an dieser Koordinate
				const float v = density(x,y);

				// Zielwert einschließlich diffundiertem Fehler
				float err = v + pRingBuf[ x ];
				if( err >= 0.5f) {
					err-= 1.0f;
					seeds.push_back(
						Eigen::Vector2f{
							static_cast<float>(x) + 0.5f,
							static_cast<float>(y) + 0.5f
						});
				}

				// Diffundierten Fehler aus dem Ringpuffer löschen,
				// damit die Speicherstelle bei einem erneuten Durchlauf durch den Ringpuffer leer ist.
				pRingBuf[ x ] = 0.0f;

				// Bei Dichte über  7% den Fehler über Radius 1 diffundieren.
				// Bei Dichte unter 7% den Fehler über Radius 2 diffundieren.
				// Bei Dichte unter 4% den Fehler über Radius 4 diffundieren.
				// Bei Dichte unter 1% den Fehler über Radius 8 diffundieren.
				const unsigned int LogTable[ 7 ] = { 3, 2, 2, 2, 1, 1, 1 };
				const int t = static_cast< int >( 100.0 * fabs( v ) );
				const unsigned int RadiusLog2 = t >= 7 ? 0 : ( t < 1 ? 3 : LogTable[ t ] );
				const unsigned int radius = 1 << RadiusLog2;

				// Dafür sorgen daß die Fehler aller Punkte innerhalb des Radius auf die
				// gleiche Koordinate diffundieren. Sonst akkumuliert sich der Fehler nie.
				// => Ausrichtung auf ein Vielfaches des Radius
				const int DiffusionX = ( x >> RadiusLog2 ) << RadiusLog2;
				const int DiffusionY = ( y >> RadiusLog2 ) << RadiusLog2;

				// Die nächsten Pixel innerhalb des Radius schneller durchlaufen.
				// Annahme: Die Dichte bleibt konstant, sodaß der Radius nicht geändert werden muß.
				// Dann können alle Fehler auf die gleichen Koordinaten diffundieren.
				++x;
				if( v > 0.5f )
				{
					// Überspringen in dichten Bereichen: Seed-Punkte erzeugen
					while( x < DiffusionX + radius )
					{
						// Fehler der übersprungenen Pixel mitnehmen.
						err += density( x, y ) + pRingBuf[ x ] - 1.0f;
						seeds.push_back(
							Eigen::Vector2f{
								static_cast<float>(x) + 0.5f,
								static_cast<float>(y) + 0.5f
							});
						pRingBuf[ x ] = 0;
						++x;
					}
				}
				else
				{
					// Überspringen in spärlichen Gebieten
					while( x < DiffusionX + radius )
					{
						// Fehler der übersprungenen Pixel mitnehmen.
						err += density( x, y ) + pRingBuf[ x ];
						pRingBuf[ x ] = 0;
						++x;
					}
				}

				// Zufällig in die eine oder andere Richtung diffundieren,
				// um Spuren zu verwischen.
				if( ( crc32 ^ radius ) & 1 )
				{
					ringbuffer( 8 + DiffusionX + radius, ( DiffusionY          ) % 8 ) += 7.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX - radius, ( DiffusionY + radius ) % 8 ) += 3.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX         , ( DiffusionY + radius ) % 8 ) += 5.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX + radius, ( DiffusionY + radius ) % 8 ) += 1.0f / 16.0f * err;
					crc32 = ( crc32 >> 1 ) ^ 0xedb88320;	// Zufallszahl aktualisieren
				}
				else
				{
					ringbuffer( 8 + DiffusionX + radius, ( DiffusionY          ) % 8 ) += 2.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX - radius, ( DiffusionY + radius ) % 8 ) += 6.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX         , ( DiffusionY + radius ) % 8 ) += 2.0f / 16.0f * err;
					ringbuffer( 8 + DiffusionX + radius, ( DiffusionY + radius ) % 8 ) += 6.0f / 16.0f * err;
					crc32 >>= 1;	// Zufallszahl aktualisieren
				}
			} // for x
		} // for y
		return seeds;
	}

}
