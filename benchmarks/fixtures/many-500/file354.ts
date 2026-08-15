interface Item354 { id: number; name: string; }
type Key354 = string | number;
export function make354(id: number, name: string): Item354 {
  const value: Item354 = {id, name};
  return value as Item354;
}
export const item354: Item354 = make354(354, "item-354");
