interface Item245 { id: number; name: string; }
type Key245 = string | number;
export function make245(id: number, name: string): Item245 {
  const value: Item245 = {id, name};
  return value as Item245;
}
export const item245: Item245 = make245(245, "item-245");
