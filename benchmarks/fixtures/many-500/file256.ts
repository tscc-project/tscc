interface Item256 { id: number; name: string; }
type Key256 = string | number;
export function make256(id: number, name: string): Item256 {
  const value: Item256 = {id, name};
  return value as Item256;
}
export const item256: Item256 = make256(256, "item-256");
