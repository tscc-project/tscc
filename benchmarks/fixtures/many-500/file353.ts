interface Item353 { id: number; name: string; }
type Key353 = string | number;
export function make353(id: number, name: string): Item353 {
  const value: Item353 = {id, name};
  return value as Item353;
}
export const item353: Item353 = make353(353, "item-353");
