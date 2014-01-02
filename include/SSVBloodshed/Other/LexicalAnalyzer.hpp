// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef OB_TESTING_OTHER_LEXICALANALYZER
#define OB_TESTING_OTHER_LEXICALANALYZER

#include <vector>
#include <memory>
#include <SSVUtils/SSVUtils.hpp>
#include "SSVBloodshed/Other/Graph.hpp"
#include "SSVBloodshed/Other/FSM.hpp"
#include "SSVBloodshed/Other/LexicalFSM.hpp"

namespace ssvut
{
	template<typename TTType, typename TTData> class LexicalAnalyzer
	{
		public:
			using LAFSM = LexicalFSM<LexicalAnalyzer<TTType, TTData>>;

			struct Token : public TTData
			{
				TTType type;
				std::string contents;

				inline Token(TTType mType, std::string mContents) : type{mType}, contents{std::move(mContents)} { }
			};

		private:
			std::map<TTType, LAFSM> matches;
			std::vector<Token> tokens;
			std::string source;
			std::size_t markerBegin, markerEnd, nextEnd;

			inline void advance() noexcept { markerEnd = nextEnd; }
			inline void consume(TTType mType)
			{
				const auto& tokenContents(source.substr(markerBegin, markerEnd - markerBegin));
				//ssvu::lo() << tokenContents << std::endl;

				tokens.emplace_back(mType, tokenContents);
				markerBegin = markerEnd;
			}

		public:
			inline LAFSM& createMatchFSM(TTType mVMToken)
			{
				assert(matches.count(mVMToken) == 0);
				matches.insert(std::make_pair(mVMToken, LAFSM{*this}));
				return matches.at(mVMToken);
			}

			inline void setSource(std::string mSource) { source = std::move(mSource); }

			inline char getMatchChar() { nextEnd = markerEnd + 1; return source[markerEnd]; }
			inline bool match(const std::string& mMatch/*, bool mConsume = true*/)
			{
				for(auto i(0u); i < mMatch.size(); ++i)
				{
					auto idxToCheck(markerEnd + i);
					if(idxToCheck >= source.size() || source[idxToCheck] != mMatch[i]) return false;
				}

				nextEnd = markerEnd + mMatch.size();
				return true;
			}
			inline bool matchAnything() { nextEnd = markerEnd + 1; return true; }

			inline void tokenize()
			{
				tokens.clear();
				markerBegin = markerEnd = nextEnd = 0;

				while(markerEnd < source.size())
				{
					bool canConsume{false};
					TTType foundType;

					for(auto& p : matches)
					{
						auto& fsm(p.second);

						fsm.reset();
						markerEnd = markerBegin;

						auto nextNode(LAFSM::getNodeNull());

						while((nextNode = fsm.getCurrentState()->getFirstMatchingTransition()) != LAFSM::getNodeNull())
						{
							advance();
							//ssvu::lo() << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;

							fsm.setCurrentState(nextNode);
							if(fsm.getCurrentState()->isTerminal())
							{
								canConsume = true;
								foundType = p.first;
							}
						}

						if(canConsume)
						{
							consume(foundType);
							break;
						}
					}

					if(canConsume) continue;

					ssvu::lo() << "didn't find any match\n" << source.substr(markerBegin, nextEnd - markerBegin) << std::endl;
					throw;
				}
			}

			inline const decltype(tokens)& getTokens() const noexcept { return tokens; }
	};
}

#endif
