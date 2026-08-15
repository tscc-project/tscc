interface Item311 { id: number; name: string; }
type Key311 = string | number;
export function make311(id: number, name: string): Item311 {
  const value: Item311 = {id, name};
  return value as Item311;
}
export const item311: Item311 = make311(311, "item-311");
