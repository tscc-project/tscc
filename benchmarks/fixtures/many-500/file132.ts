interface Item132 { id: number; name: string; }
type Key132 = string | number;
export function make132(id: number, name: string): Item132 {
  const value: Item132 = {id, name};
  return value as Item132;
}
export const item132: Item132 = make132(132, "item-132");
