interface Item432 { id: number; name: string; }
type Key432 = string | number;
export function make432(id: number, name: string): Item432 {
  const value: Item432 = {id, name};
  return value as Item432;
}
export const item432: Item432 = make432(432, "item-432");
