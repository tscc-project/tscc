interface Item421 { id: number; name: string; }
type Key421 = string | number;
export function make421(id: number, name: string): Item421 {
  const value: Item421 = {id, name};
  return value as Item421;
}
export const item421: Item421 = make421(421, "item-421");
