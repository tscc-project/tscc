interface Item105 { id: number; name: string; }
type Key105 = string | number;
export function make105(id: number, name: string): Item105 {
  const value: Item105 = {id, name};
  return value as Item105;
}
export const item105: Item105 = make105(105, "item-105");
