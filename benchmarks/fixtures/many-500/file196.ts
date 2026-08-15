interface Item196 { id: number; name: string; }
type Key196 = string | number;
export function make196(id: number, name: string): Item196 {
  const value: Item196 = {id, name};
  return value as Item196;
}
export const item196: Item196 = make196(196, "item-196");
