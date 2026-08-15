interface Item283 { id: number; name: string; }
type Key283 = string | number;
export function make283(id: number, name: string): Item283 {
  const value: Item283 = {id, name};
  return value as Item283;
}
export const item283: Item283 = make283(283, "item-283");
