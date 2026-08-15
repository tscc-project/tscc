interface Item485 { id: number; name: string; }
type Key485 = string | number;
export function make485(id: number, name: string): Item485 {
  const value: Item485 = {id, name};
  return value as Item485;
}
export const item485: Item485 = make485(485, "item-485");
