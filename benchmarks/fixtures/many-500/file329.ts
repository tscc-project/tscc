interface Item329 { id: number; name: string; }
type Key329 = string | number;
export function make329(id: number, name: string): Item329 {
  const value: Item329 = {id, name};
  return value as Item329;
}
export const item329: Item329 = make329(329, "item-329");
