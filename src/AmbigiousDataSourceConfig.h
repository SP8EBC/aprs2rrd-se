/*
 * AmbigiousDataSourceConfig.h
 *
 *  Created on: Jan 2, 2022
 *      Author: mateusz
 */

#ifndef AMBIGIOUSDATASOURCECONFIG_H_
#define AMBIGIOUSDATASOURCECONFIG_H_

class AmbigiousDataSourceConfig {
public:
	AmbigiousDataSourceConfig();
	virtual ~AmbigiousDataSourceConfig();
	AmbigiousDataSourceConfig(const AmbigiousDataSourceConfig &other);
	AmbigiousDataSourceConfig(AmbigiousDataSourceConfig &&other);
	AmbigiousDataSourceConfig& operator=(
			const AmbigiousDataSourceConfig &other);
	AmbigiousDataSourceConfig& operator=(AmbigiousDataSourceConfig &&other);
};

#endif /* AMBIGIOUSDATASOURCECONFIG_H_ */
